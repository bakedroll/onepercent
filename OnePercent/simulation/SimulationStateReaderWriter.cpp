#include "SimulationStateReaderWriter.h"

#include "core/Macros.h"
#include "simulation/SimulationStateContainer.h"
#include "simulation/SimulationState.h"
#include "simulation/UpdateThread.h"

#include <QFile>
#include <QMessageBox>


namespace onep
{
  SimulationStateReaderWriter::SimulationStateReaderWriter()
  {
  }

  SimulationStateReaderWriter::~SimulationStateReaderWriter()
  {
  }

  bool SimulationStateReaderWriter::saveState(
    const std::string& filename,
    osg::ref_ptr<SimulationStateContainer> stateContainer,
    osg::ref_ptr<SkillsContainer> skillsContainer,
    osgGaming::Observable<int>::Ptr oDay,
    osgGaming::Observable<int>::Ptr oNumSkillPoints)
  {
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::WriteOnly))
    {
      QMessageBox::critical(nullptr, "Save state", "Could not write file.");
      return false;
    }

    QDataStream stream(&file);

    stateContainer->accessState([&](SimulationState::Ptr state)
    {
      CountryState::Map& cstates = state->getCountryStates();

      int numBranches = skillsContainer->getNumBranches();

      stream << int(cstates.size());
      for (CountryState::Map::iterator it = cstates.begin(); it != cstates.end(); ++it)
      {
        stream << it->first;

        CountryState::ValuesMap& values = it->second->getValuesMap();
        CountryState::BranchValuesMap& branchValues = it->second->getBranchValuesMap();

        stream << int(values.size());
        for (CountryState::ValuesMap::iterator vit = values.begin(); vit != values.end(); ++vit)
        {
          stream << int(vit->first.length());
          stream.writeRawData(vit->first.c_str(), vit->first.length());

          stream << float(vit->second);
        }

        stream << int(branchValues.size());
        for (CountryState::BranchValuesMap::iterator bit = branchValues.begin(); bit != branchValues.end(); ++bit)
        {
          stream << int(bit->first.length());
          stream.writeRawData(bit->first.c_str(), bit->first.length());

          stream << int(bit->second.size());
          for (CountryState::ValuesMap::iterator vit = bit->second.begin(); vit != bit->second.end(); ++vit)
          {
            stream << int(vit->first.length());
            stream.writeRawData(vit->first.c_str(), vit->first.length());

            stream << float(vit->second);
          }
        }

        stream << numBranches;
        for (int i = 0; i < numBranches; i++)
        {
          std::string branchName = skillsContainer->getBranchByIndex(i)->getBranchName();

          stream << int(branchName.length());
          stream.writeRawData(branchName.c_str(), branchName.length());

          bool activated = it->second->getBranchActivated(branchName.c_str());

          stream << activated;
        }
      }

      stream << numBranches;
      for (int i = 0; i < numBranches; i++)
      {
        SkillBranch::Ptr branch = skillsContainer->getBranchByIndex(i);
        std::string branchName = branch->getBranchName();

        stream << int(branchName.length());
        stream.writeRawData(branchName.c_str(), branchName.length());

        int numSkills = branch->getNumSkills();
        stream << numSkills;

        for (int j = 0; j < numSkills; j++)
        {
          Skill::Ptr skill = branch->getSkillByIndex(j);
          std::string skillName = skill->getSkillName();

          stream << int(skillName.length());
          stream.writeRawData(skillName.c_str(), skillName.length());

          stream << skill->getObActivated()->get();
        }
      }

      stream << oNumSkillPoints->get();
      stream << oDay->get();
    });

    file.close();
    return true;
  }

  bool SimulationStateReaderWriter::loadState(
    const std::string& filename,
    osg::ref_ptr<SimulationStateContainer> stateContainer,
    osg::ref_ptr<SkillsContainer> skillsContainer,
    osgGaming::Observable<int>::Ptr oDay,
    osgGaming::Observable<int>::Ptr oNumSkillPoints,
    UpdateThread* thread)
  {
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::ReadOnly))
    {
      QMessageBox::critical(nullptr, "Save state", "Could not open file.");
      return false;
    }

    QDataStream stream(&file);

    thread->executeLockedTick([&]()
    {
      stateContainer->accessState([&](SimulationState::Ptr state)
      {
        CountryState::Map& cstates = state->getCountryStates();
        int numCountries, cid, numValues, numBranches, numSkills, len;
        char buffer[256];
        float value;
        bool activated;

        stream >> numCountries;
        for (int i = 0; i < numCountries; i++)
        {
          stream >> cid;

          if (cstates.count(cid) == 0)
          {
            OSGG_QLOG_WARN(QString("No country with id %1").arg(cid));
            return;
          }

          CountryState::Ptr cstate = cstates[cid];

          CountryState::ValuesMap& values = cstate->getValuesMap();
          CountryState::BranchValuesMap& branchValues = cstate->getBranchValuesMap();

          stream >> numValues;
          for (int j = 0; j < numValues; j++)
          {
            stream >> len; stream.readRawData(buffer, len); buffer[len] = '\0';
            std::string valueName(buffer);

            stream >> value;

            if (values.count(valueName) == 0)
            {
              OSGG_QLOG_WARN(QString("Value %1 not found").arg(valueName.c_str()));
              continue;
            }

            values[valueName] = value;
          }

          stream >> numBranches;
          for (int j = 0; j < numBranches; j++)
          {
            stream >> len; stream.readRawData(buffer, len); buffer[len] = '\0';
            std::string branchName(buffer);

            stream >> numValues;
            for (int k = 0; k < numValues; k++)
            {
              stream >> len; stream.readRawData(buffer, len); buffer[len] = '\0';
              std::string valueName(buffer);

              stream >> value;

              if (branchValues.count(branchName) == 0)
              {
                OSGG_QLOG_WARN(QString("Branch %1 not found").arg(branchName.c_str()));
                continue;
              }

              if (branchValues[branchName].count(valueName) == 0)
              {
                OSGG_QLOG_WARN(QString("Value %1 not found").arg(valueName.c_str()));
                continue;
              }

              branchValues[branchName][valueName] = value;
            }
          }

          stream >> numBranches;
          for (int j = 0; j < numBranches; j++)
          {
            stream >> len; stream.readRawData(buffer, len); buffer[len] = '\0';
            std::string branchName(buffer);

            stream >> activated;

            if (skillsContainer->getBranchByName(branchName) == nullptr)
            {
              OSGG_QLOG_WARN(QString("Branch not found"));
              continue;
            }

            osgGaming::Observable<bool>::Ptr obs = cstate->getOActivatedBranch(branchName.c_str());
            if (obs->get() != activated) obs->set(activated);
          }
        }

        stream >> numBranches;
        for (int i = 0; i < numBranches; i++)
        {
          stream >> len; stream.readRawData(buffer, len); buffer[len] = '\0';
          std::string branchName(buffer);

          SkillBranch::Ptr branch = skillsContainer->getBranchByName(branchName);

          stream >> numSkills;
          for (int j = 0; j < numSkills; j++)
          {
            stream >> len; stream.readRawData(buffer, len); buffer[len] = '\0';
            std::string skillName(buffer);

            stream >> activated;

            if (!branch.valid() || !branch->getSkillByName(skillName))
            {
              OSGG_QLOG_WARN(QString("Skill %1 not found").arg(skillName.c_str()));
              continue;
            }

            Skill::Ptr skill = branch->getSkillByName(skillName);
            if (activated != skill->getObActivated()->get()) skill->getObActivated()->set(activated);
          }
        }

        thread->executeLockedLuaState([&state](){ state->write(); });
      });
    });

    int number;
    stream >> number;
    oNumSkillPoints->set(number);
    stream >> number;
    oDay->set(number);

    file.close();
    return true;
  }
}