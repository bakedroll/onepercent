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
  SimulationStateReaderWriter::SimulationStateReaderWriter() = default;

  SimulationStateReaderWriter::~SimulationStateReaderWriter() = default;

  bool SimulationStateReaderWriter::saveState(
    const std::string& filename,
    const osg::ref_ptr<ModelContainer>& modelContainer,
    const osgGaming::Observable<int>::Ptr& oDay,
    const osgGaming::Observable<int>::Ptr& oNumSkillPoints)
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
      const auto& cstates     = model->getSimulationStateTable()->getCountryStates();
      const auto& branches    = model->getBranchesTable()->getBranches();
      const auto  numBranches = static_cast<int>(branches.size());

      stream << static_cast<int>(cstates.size());
      for (const auto& cstate : cstates)
      {
        stream << cstate.first;

        auto values       = cstate.second->getValuesMap();
        auto branchValues = cstate.second->getBranchValuesMap();

        stream << static_cast<int>(values.size());
        for (const auto& value : values)
        {
          stream << static_cast<int>(value.first.length());
          stream.writeRawData(value.first.c_str(), static_cast<int>(value.first.length()));

          stream << static_cast<float>(value.second);
        }

        stream << static_cast<int>(branchValues.size());
        for (const auto& branchValue : branchValues)
        {
          stream << static_cast<int>(branchValue.first.length());
          stream.writeRawData(branchValue.first.c_str(), static_cast<int>(branchValue.first.length()));

          stream << static_cast<int>(branchValue.second.size());
          for (const auto& vit : branchValue.second)
          {
            stream << static_cast<int>(vit.first.length());
            stream.writeRawData(vit.first.c_str(), static_cast<int>(vit.first.length()));

            stream << float(vit.second);
          }
        }

        stream << numBranches;
        for (const auto& branch : branches)
        {
          const auto& branchName = branch.second->getName();

          stream << static_cast<int>(branchName.length());
          stream.writeRawData(branchName.c_str(), static_cast<int>(branchName.length()));

          const auto activated = cstate.second->getBranchesActivatedTable()->getBranchActivated(branchName);

          stream << activated;
        }
      }

      stream << numBranches;
      for (const auto& branch : branches)
      {
        const auto& branchName = branch.second->getName();

        stream << static_cast<int>(branchName.length());
        stream.writeRawData(branchName.c_str(), static_cast<int>(branchName.length()));

        auto numSkills = branch.second->getSkillsTable()->getNumSkills();
        stream << numSkills;

        for (auto j = 0; j < numSkills; j++)
        {
          auto        skill     = branch.second->getSkillsTable()->getSkillByIndex(j);
          const auto& skillName = skill->getName();

          stream << static_cast<int>(skillName.length());
          stream.writeRawData(skillName.c_str(), static_cast<int>(skillName.length()));

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
    const osg::ref_ptr<ModelContainer>& modelContainer,
    const osgGaming::Observable<int>::Ptr& oDay,
    const osgGaming::Observable<int>::Ptr& oNumSkillPoints,
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
        auto& cstates = model->getSimulationStateTable()->getCountryStates();

        int   numCountries, cid, numValues, numBranches, numSkills, len;
        char  buffer[256];
        float value;
        bool  activated;

        auto branchesTable = model->getBranchesTable();

        stream >> numCountries;
        for (auto i = 0; i < numCountries; i++)
        {
          stream >> cid;

          if (cstates.count(cid) == 0)
          {
            OSGG_QLOG_WARN(QString("No country with id %1").arg(cid));
            return;
          }

          auto& cstate       = cstates[cid];
          auto  values       = cstate->getValuesMap();
          auto  branchValues = cstate->getBranchValuesMap();

          stream >> numValues;
          for (auto j = 0; j < numValues; j++)
          {
            stream >> len;
            stream.readRawData(buffer, len);
            buffer[len] = '\0';
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
          for (auto j = 0; j < numBranches; j++)
          {
            stream >> len;
            stream.readRawData(buffer, len);
            buffer[len] = '\0';
            std::string branchName(buffer);

            stream >> numValues;
            for (auto k = 0; k < numValues; k++)
            {
              stream >> len;
              stream.readRawData(buffer, len);
              buffer[len] = '\0';
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
          for (auto j = 0; j < numBranches; j++)
          {
            stream >> len;
            stream.readRawData(buffer, len);
            buffer[len] = '\0';
            std::string branchName(buffer);

            stream >> activated;

            if (branchesTable->getBranchByName(branchName) == nullptr)
            {
              OSGG_QLOG_WARN(QString("Branch not found"));
              continue;
            }

            if (activated != cstate->getBranchesActivatedTable()->getBranchActivated(branchName))
            {
              cstate->getBranchesActivatedTable()->setBranchActivated(branchName, activated);
            }
          }
        }

        stream >> numBranches;
        for (auto i = 0; i < numBranches; i++)
        {
          stream >> len;
          stream.readRawData(buffer, len);
          buffer[len] = '\0';
          std::string branchName(buffer);

          auto branch = branchesTable->getBranchByName(branchName);

          stream >> numSkills;
          for (auto j = 0; j < numSkills; j++)
          {
            stream >> len;
            stream.readRawData(buffer, len);
            buffer[len] = '\0';
            std::string skillName(buffer);

            stream >> activated;

            if (!branch || !branch->getSkillsTable()->getSkillByName(skillName))
            {
              OSGG_QLOG_WARN(QString("Skill %1 not found").arg(skillName.c_str()));
              continue;
            }

            auto skill = branch->getSkillsTable()->getSkillByName(skillName);
            if (activated != skill->getIsActivated())
            {
              skill->setIsActivated(activated);
            }
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