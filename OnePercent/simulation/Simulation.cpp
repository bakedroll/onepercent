#include "Simulation.h"

#include "core/QConnectFunctor.h"
#include "core/Multithreading.h"
#include "core/Observables.h"
#include "core/Macros.h"
#include "simulation/SkillsContainer.h"
#include "simulation/SimulationStateContainer.h"
#include "simulation/CountryState.h"
#include "simulation/SkillBranch.h"
#include "simulation/UpdateThread.h"

#include <QTimer>
#include <QFile>
#include <QMessageBox>

extern "C"
{
#include <lua.h>
}

#include <LuaBridge.h>

namespace onep
{
  struct Simulation::Impl
  {
    Impl(osgGaming::Injector& injector)
      : lua(injector.inject<LuaStateManager>())
      , skillsContainer(injector.inject<SkillsContainer>())
      , stateContainer(injector.inject<SimulationStateContainer>())
      , oDay(injector.inject<ODay>())
      , oNumSkillPoints(injector.inject<ONumSkillPoints>())
      , oRunning(new osgGaming::Observable<bool>(false))
      , bSyncing(false)
    {}

    osg::ref_ptr<LuaStateManager> lua;

    SkillsContainer::Ptr skillsContainer;
    SimulationStateContainer::Ptr stateContainer;

    QTimer timer;

    ODay::Ptr oDay;
    ONumSkillPoints::Ptr oNumSkillPoints;

    LuaRefPtr refUpdate_skills_func;
    LuaRefPtr refUpdate_branches_func;
    LuaRefPtr refDump_object;

    osgGaming::Observable<bool>::Ptr oRunning;

    std::vector<osgGaming::Observer<bool>::Ptr> notifyActivatedList;

    UpdateThread thread;

    bool bSyncing;
  };

  Simulation::Simulation(osgGaming::Injector& injector)
    : osg::Referenced()
    , LuaClassInstance()
    , m(new Impl(injector))

  {
    // start with 50 skill points
    m->oNumSkillPoints->set(50);

    m->timer.setSingleShot(false);
    m->timer.setInterval(1000);

    QConnectFunctor::connect(&m->timer, SIGNAL(timeout()), [this]()
    {
      m->thread.doNextStep();
    });

    QConnectFunctor::connect(&m->thread, SIGNAL(onStateUpdated()), [this]()
    {
      // synchronize with lua state
      // Caution: lua state mutex is locked here
      m->bSyncing = true;
      m->stateContainer->getState()->read();
      m->bSyncing = false;

      // increment day
      m->oDay->set(m->oDay->get() + 1);
    }, nullptr, Qt::BlockingQueuedConnection);
  }

  Simulation::~Simulation()
  {
  }

  void Simulation::prepare()
  {
    m->refUpdate_skills_func    = MAKE_LUAREF_PTR(m->lua->getObject("core.control.update_skills_func"));
    m->refUpdate_branches_func  = MAKE_LUAREF_PTR(m->lua->getObject("core.control.update_branches_func"));
    m->refDump_object           = MAKE_LUAREF_PTR(m->lua->getObject("core.helper.dump_object"));

    if (!m->refUpdate_skills_func->isFunction())
      OSGG_QLOG_FATAL(QString("Could not load lua function ''").arg("update_skills_func"));

    if (!m->refUpdate_branches_func->isFunction())
      OSGG_QLOG_FATAL(QString("Could not load lua function ''").arg("update_branches_func"));

    if (!m->refDump_object->isFunction())
      OSGG_QLOG_FATAL(QString("Could not load lua function ''").arg("dump_object"));

    int nBranches = m->skillsContainer->getNumBranches();

    ONEP_FOREACH(CountryState::Map, it, m->stateContainer->getState()->getCountryStates())
    {
      CountryState::Ptr cstate = it->second;

      for (int i = 0; i < nBranches; i++)
      {
        std::string branchName = m->skillsContainer->getBranchByIndex(i)->getBranchName();

        m->notifyActivatedList.push_back(cstate->getOActivatedBranch(branchName.c_str())->connect(osgGaming::Func<bool>([this, cstate](bool activated)
        {
          if (m->bSyncing)
            return;

          m->thread.scheduleLuaTask([cstate](){ cstate->writeBranchesActivated(); });
        })));
      }
      
    }

    for (int i = 0; i < nBranches; i++)
    {
      SkillBranch::Ptr branch = m->skillsContainer->getBranchByIndex(i);
     
      int nskills = branch->getNumSkills();
      for (int j = 0; j < nskills; j++)
      {
        Skill::Ptr skill = branch->getSkillByIndex(j);
        m->notifyActivatedList.push_back(skill->getObActivated()->connect(osgGaming::Func<bool>([this, skill](bool activated)
        {
          m->thread.scheduleLuaTask([skill](){ skill->write(); });
        })));
      }
    }

    m->thread.setUpdateFunctions(m->refUpdate_skills_func, m->refUpdate_branches_func);
    m->thread.start();
  }

  void Simulation::shutdownUpdateThread()
  {
    m->thread.shutdown();
    m->thread.doNextStep();
    m->thread.wait();
  }

  UpdateThread* Simulation::getUpdateThread()
  {
    return &m->thread;
  }

  bool Simulation::paySkillPoints(int points)
  {
    int amount = m->oNumSkillPoints->get();
    if (amount < points)
      return false;

    m->oNumSkillPoints->set(amount - points);
    return true;
  }

  void Simulation::start()
  {
    m->oRunning->set(true);
    m->timer.start();
    OSGG_LOG_DEBUG("Simulation started");
  }

  void Simulation::stop()
  {
    m->timer.stop();
    m->oRunning->set(false);
    OSGG_LOG_DEBUG("Simulation stopped");
  }

  bool Simulation::running() const
  {
    return m->timer.isActive();
  }

  osgGaming::Observable<bool>::Ptr Simulation::getORunning() const
  {
    return m->oRunning;
  }

  void Simulation::saveState(std::string filename)
  {
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::WriteOnly))
    {
      QMessageBox::critical(nullptr, "Save state", "Could not write file.");
      return;
    }

    QDataStream stream(&file);

    CountryState::Map& state = m->stateContainer->getState()->getCountryStates();

    int numBranches = m->skillsContainer->getNumBranches();

    stream << int(state.size());
    for (CountryState::Map::iterator it = state.begin(); it != state.end(); ++it)
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
        std::string branchName = m->skillsContainer->getBranchByIndex(i)->getBranchName();

        stream << int(branchName.length());
        stream.writeRawData(branchName.c_str(), branchName.length());

        bool activated = it->second->getBranchActivated(branchName.c_str());

        stream << activated;
      }
    }

    stream << numBranches;
    for (int i = 0; i < numBranches; i++)
    {
      SkillBranch::Ptr branch = m->skillsContainer->getBranchByIndex(i);
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

    stream << m->oNumSkillPoints->get();
    stream << m->oDay->get();

    file.close();
  }

  void Simulation::loadState(std::string filename)
  {
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::ReadOnly))
    {
      QMessageBox::critical(nullptr, "Save state", "Could not open file.");
      return;
    }

    QDataStream stream(&file);
    m->bSyncing = true;

    m->thread.executeLuaTask([this, &stream]()
    {
      CountryState::Map& state = m->stateContainer->getState()->getCountryStates();
      int numCountries, cid, numValues, numBranches, numSkills, len, number;
      char buffer[256];
      float value;
      bool activated;

      stream >> numCountries;
      for (int i = 0; i < numCountries; i++)
      {
        stream >> cid;

        if (state.count(cid) == 0)
        {
          OSGG_QLOG_WARN(QString("No country with id %1").arg(cid));
          return;
        }

        CountryState::Ptr cstate = state[cid];

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

          if (m->skillsContainer->getBranchByName(branchName) == nullptr)
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

        SkillBranch::Ptr branch = m->skillsContainer->getBranchByName(branchName);

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

      stream >> number;
      m->oNumSkillPoints->set(number);
      stream >> number;
      m->oDay->set(number);

      m->stateContainer->getState()->write();
    });

    file.close();
    m->bSyncing = false;
  }

  void Simulation::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<Simulation>("Simulation")
      .addFunction("start", &Simulation::lua_start)
      .addFunction("stop", &Simulation::lua_stop)
      .addFunction("set_day", &Simulation::lua_set_day)
      .addFunction("set_interval", &Simulation::lua_set_interval)
      .addFunction("set_skill_points", &Simulation::lua_set_skill_points)
      .addFunction("add_skill_points", &Simulation::lua_add_skill_points)
    .endClass();
  }

  std::string Simulation::instanceVariableName()
  {
    return "simulation";
  }

  void Simulation::lua_start(lua_State* state)
  {
    Multithreading::uiExecuteOrAsync([this](){ start(); });
  }

  void Simulation::lua_stop(lua_State* state)
  {
    Multithreading::uiExecuteOrAsync([this](){ stop(); });
  }

  void Simulation::lua_set_skill_points(int points)
  {
    OSGG_QLOG_DEBUG(QString("Set skill points: %1").arg(points));
    Multithreading::uiExecuteOrAsync([=](){ m->oNumSkillPoints->set(points); });
  }

  void Simulation::lua_add_skill_points(int points)
  {
    OSGG_QLOG_DEBUG(QString("Add skill points: %1").arg(points));
    Multithreading::uiExecuteOrAsync([=]()
    {
      int p = m->oNumSkillPoints->get();
      m->oNumSkillPoints->set(p + points);
    });
  }

  void Simulation::lua_set_day(int day)
  {
    OSGG_QLOG_DEBUG(QString("Set day: %1").arg(day));
    Multithreading::uiExecuteOrAsync([=](){ m->oDay->set(day); });
  }

  void Simulation::lua_set_interval(int interval)
  {
    OSGG_QLOG_DEBUG(QString("Set interval: %1").arg(interval));
    Multithreading::uiExecuteOrAsync([=](){ m->timer.setInterval(interval); });
  }
}