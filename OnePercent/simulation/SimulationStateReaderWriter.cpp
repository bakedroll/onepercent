#include "SimulationStateReaderWriter.h"

#include "core/Macros.h"
#include "scripting/LuaSimulationStateTable.h"
#include "scripting/LuaSkillsTable.h"
#include "scripting/LuaModel.h"
#include "scripting/LuaBranchesTable.h"
#include "simulation/UpdateThread.h"
#include "simulation/ModelContainer.h"

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
    osg::ref_ptr<ModelContainer> modelContainer,
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

    modelContainer->accessModel([&](const LuaModel::Ptr& model)
    {
      LuaCountryState::Map& cstates = model->getSimulationStateTable()->getCountryStates();

      auto branchesTable = model->getBranchesTable();
      int numBranches = branchesTable->getNumBranches();

      stream << int(cstates.size());
      for (LuaCountryState::Map::iterator it = cstates.begin(); it != cstates.end(); ++it)
      {
        stream << it->first;

        auto values = it->second->getValuesMap();
        auto branchValues = it->second->getBranchValuesMap();

        stream << int(values.size());
        for (auto vit = values.begin(); vit != values.end(); ++vit)
        {
          stream << int(vit->first.length());
          stream.writeRawData(vit->first.c_str(), vit->first.length());

          stream << float(vit->second);
        }

        stream << int(branchValues.size());
        for (auto bit = branchValues.begin(); bit != branchValues.end(); ++bit)
        {
          stream << int(bit->first.length());
          stream.writeRawData(bit->first.c_str(), bit->first.length());

          stream << int(bit->second.size());
          for (auto vit = bit->second.begin(); vit != bit->second.end(); ++vit)
          {
            stream << int(vit->first.length());
            stream.writeRawData(vit->first.c_str(), vit->first.length());

            stream << float(vit->second);
          }
        }

        stream << numBranches;
        for (int i = 0; i < numBranches; i++)
        {
          std::string branchName = branchesTable->getBranchByIndex(i)->getName();

          stream << int(branchName.length());
          stream.writeRawData(branchName.c_str(), branchName.length());

          bool activated = it->second->getBranchesActivatedTable()->getBranchActivated(branchName.c_str());

          stream << activated;
        }
      }

      stream << numBranches;
      for (int i = 0; i < numBranches; i++)
      {
        LuaSkillBranch::Ptr branch = branchesTable->getBranchByIndex(i);
        std::string branchName = branch->getName();

        stream << int(branchName.length());
        stream.writeRawData(branchName.c_str(), branchName.length());

        int numSkills = branch->getSkillsTable()->getNumSkills();
        stream << numSkills;

        for (int j = 0; j < numSkills; j++)
        {
          LuaSkill::Ptr skill = branch->getSkillsTable()->getSkillByIndex(j);
          std::string skillName = skill->getName();

          stream << int(skillName.length());
          stream.writeRawData(skillName.c_str(), skillName.length());

          stream << skill->getIsActivated();
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
    osg::ref_ptr<ModelContainer> modelContainer,
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
      modelContainer->accessModel([&](const LuaModel::Ptr& model)
      {
        LuaCountryState::Map& cstates = model->getSimulationStateTable()->getCountryStates();
        int numCountries, cid, numValues, numBranches, numSkills, len;
        char buffer[256];
        float value;
        bool activated;

        auto branchesTable = model->getBranchesTable();

        stream >> numCountries;
        for (int i = 0; i < numCountries; i++)
        {
          stream >> cid;

          if (cstates.count(cid) == 0)
          {
            OSGG_QLOG_WARN(QString("No country with id %1").arg(cid));
            return;
          }

          LuaCountryState::Ptr cstate = cstates[cid];

          auto values = cstate->getValuesMap();
          auto branchValues = cstate->getBranchValuesMap();

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

            cstate->getValuesTable()->setValue(valueName, value);
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

              cstate->getBranchValuesTable()->getBranch(branchName)->setValue(valueName, value);
            }
          }

          stream >> numBranches;
          for (int j = 0; j < numBranches; j++)
          {
            stream >> len; stream.readRawData(buffer, len); buffer[len] = '\0';
            std::string branchName(buffer);

            stream >> activated;

            if (branchesTable->getBranchByName(branchName) == nullptr)
            {
              OSGG_QLOG_WARN(QString("Branch not found"));
              continue;
            }

            if (activated != cstate->getBranchesActivatedTable()->getBranchActivated(branchName))
              cstate->getBranchesActivatedTable()->setBranchActivated(branchName, activated);
          }
        }

        stream >> numBranches;
        for (int i = 0; i < numBranches; i++)
        {
          stream >> len; stream.readRawData(buffer, len); buffer[len] = '\0';
          std::string branchName(buffer);

          LuaSkillBranch::Ptr branch = branchesTable->getBranchByName(branchName);

          stream >> numSkills;
          for (int j = 0; j < numSkills; j++)
          {
            stream >> len; stream.readRawData(buffer, len); buffer[len] = '\0';
            std::string skillName(buffer);

            stream >> activated;

            if (!branch || !branch->getSkillsTable()->getSkillByName(skillName))
            {
              OSGG_QLOG_WARN(QString("Skill %1 not found").arg(skillName.c_str()));
              continue;
            }

            LuaSkill::Ptr skill = branch->getSkillsTable()->getSkillByName(skillName);
            if (activated != skill->getIsActivated())
              skill->setIsActivated(activated);
          }
        }
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