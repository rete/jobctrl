/*
 *
 * ProcessManager.cc source template automatically generated by a class generator
 * Creation date : lun. sept. 26 2016
 *
 * This file is part of procctrl libraries.
 * 
 * procctrl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * procctrl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with procctrl.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete
 * @copyright Remi Ete
 */


#include "ProcessManager.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <vector>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

namespace procctrl {

  namespace server {

    ProcessManager::ProcessManager()
    {
      /* nop */
    }

    //----------------------------------------------------------------------------------

    ProcessManager::~ProcessManager()
    {
      this->killAllProcesses();
    }

    //----------------------------------------------------------------------------------

    Status ProcessManager::addProcess(
        const std::string &name,
        const std::string &group,
        const std::string &program,
        const ArgumentList &args,
        const Environnement &env
    )
    {
      if(m_processes.find(name) != m_processes.end())
        return ALREADY_EXISTS;

      Process process;

      process.m_name = name;
      process.m_group = group;
      process.m_program = program;
      process.m_arguments = args;
      process.m_environement = env;
      process.m_pid = 0;

      m_processes[name] = process;

      return SUCCESS;
    }

    //----------------------------------------------------------------------------------

    Status ProcessManager::startProcess(
        const std::string &name
    )
    {
      ProcessMap::iterator iter = m_processes.find(name);

      if(iter == m_processes.end())
        return NOT_FOUND;

      if(iter->second.m_pid != 0)
        return ALREADY_RUNNING;

      return this->startProcess(iter->second);
    }

    //----------------------------------------------------------------------------------

    Status ProcessManager::restartProcess(
        const std::string &name,
        KillSignal sig
    )
    {
      Status status(this->killProcess(name, sig));

      if(status)
        return status;

      return this->startProcess(name);
    }

    //----------------------------------------------------------------------------------

    Status ProcessManager::removeProcess(
        const std::string &name,
        KillSignal sig
    )
    {
      Status status(this->killProcess(name, sig));

      if(status)
        return status;

      ProcessMap::iterator iter = m_processes.find(name);

      if(iter == m_processes.end())
        return NOT_FOUND;

      m_processes.erase(iter);

      return SUCCESS;
    }

    //----------------------------------------------------------------------------------

    Status ProcessManager::killProcess(
        const std::string &name,
        KillSignal sig
    )
    {
      ProcessMap::iterator iter = m_processes.find(name);

      if(iter == m_processes.end())
        return NOT_FOUND;

      if(iter->second.m_pid != 0)
        ::kill(iter->second.m_pid, static_cast<int>(sig));

      return SUCCESS;
    }

    //----------------------------------------------------------------------------------

    Status ProcessManager::killAllProcesses(
        KillSignal sig
    )
    {
      for(auto &j : m_processes)
      {
        if(j.second.m_pid != 0)
          ::kill(j.second.m_pid, static_cast<int>(sig));
      }

      return SUCCESS;
    }

    //----------------------------------------------------------------------------------

    ProcessStatus ProcessManager::getProcessStatus(
        const std::string &name
    ) const
    {
      ProcessMap::const_iterator iter = m_processes.find(name);

      if(iter == m_processes.end())
        return UNDEFINED;

      return iter->second.m_status;
    }

    //----------------------------------------------------------------------------------

    bool ProcessManager::isProcessRegistered(
        const std::string &name
    ) const
    {
      return m_processes.find(name) != m_processes.end();
    }

    //----------------------------------------------------------------------------------

    Status ProcessManager::getProcessGroup(
        const std::string &name,
        std::string &group
    ) const
    {
      ProcessMap::const_iterator iter = m_processes.find(name);

      if(iter == m_processes.end())
        return NOT_FOUND;

      group = iter->second.m_group;

      return SUCCESS;
    }

    //----------------------------------------------------------------------------------

    std::vector<std::string> ProcessManager::getProcessNames() const
    {
      std::vector<std::string> processNames;

      for(auto &j : m_processes)
        processNames.push_back(j.first);

      return processNames;
    }

    //----------------------------------------------------------------------------------

    std::vector<std::string> ProcessManager::getProcessNames(
        const std::string &group
    ) const
    {
      std::vector<std::string> processNames;

      for(auto &j : m_processes)
        if(j.second.m_group == group)
          processNames.push_back(j.first);

      return processNames;
    }

    //----------------------------------------------------------------------------------

    std::string ProcessManager::getProcessLogFile(
        const std::string &name
    ) const
    {
      ProcessMap::const_iterator iter = m_processes.find(name);

      if(iter == m_processes.end())
        return "No such job with name '" + name + "'";

      std::stringstream fileName;
      fileName << "/tmp/jobctl" << iter->second.m_pid << ".log";
      std::ifstream in(fileName.str().c_str(), std::ios::in);

      if ( in )
      {
        std::string contents;

        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();

        return contents;
      }
      else
        return "File '" + fileName.str() + "' not found !";
    }

    //----------------------------------------------------------------------------------

    Status ProcessManager::modifyEnvironement(
        const std::string &name,
        const Environnement &env
    )
    {
      ProcessMap::iterator iter = m_processes.find(name);

      if(iter == m_processes.end())
        return NOT_FOUND;

      if(iter->second.m_pid != 0)
        return NOT_ALLOWED;

      iter->second.m_environement = env;

      return SUCCESS;
    }

    //----------------------------------------------------------------------------------

    Status ProcessManager::modifyProgram(
        const std::string &name,
        const std::string &program
    )
    {
      ProcessMap::iterator iter = m_processes.find(name);

      if(iter == m_processes.end())
        return NOT_FOUND;

      if(iter->second.m_pid != 0)
        return NOT_ALLOWED;

      iter->second.m_program = program;

      return SUCCESS;
    }

    //----------------------------------------------------------------------------------

    Status ProcessManager::modifyArguments(
        const std::string &name,
        const ArgumentList &args
    )
    {
      ProcessMap::iterator iter = m_processes.find(name);

      if(iter == m_processes.end())
        return NOT_FOUND;

      if(iter->second.m_pid != 0)
        return NOT_ALLOWED;

      iter->second.m_arguments = args;

      return SUCCESS;
    }

    //----------------------------------------------------------------------------------

    Status ProcessManager::startProcess(
        Process &process
    )
    {
      if (process.m_pid != 0)
        return ALREADY_RUNNING;

      // check input
      if(process.m_program.size() > PROCCTRL_MAX_SIZE)
        return FAILURE;

      for(auto &arg : process.m_arguments)
        if(arg.size() > PROCCTRL_MAX_SIZE)
          return FAILURE;

      for(auto &env : process.m_environement)
        if(env.first.size() + env.second.size() + 1 > PROCCTRL_MAX_SIZE)
          return FAILURE;

      if(process.m_arguments.size() > PROCCTRL_MAX_NARGS-2)
        return FAILURE;

      if(process.m_environement.size() > PROCCTRL_MAX_NENV-1)
        return FAILURE;



      signal(SIGCHLD, SIG_IGN);

      // forking
      pid_t pid = fork();

      // parent case
      if (pid != 0)
      {
        process.m_pid = pid;
        process.m_status = this->getProcessStatus(pid);

        return SUCCESS;
      }

      // program name
      char program[PROCCTRL_MAX_SIZE];
      sprintf(program, process.m_program.c_str());

      char argv[PROCCTRL_MAX_NARGS][PROCCTRL_MAX_SIZE];

      // fills arguments list
      for (int i = 0; i < PROCCTRL_MAX_NARGS ; i++)
        for (int j = 0; j < PROCCTRL_MAX_SIZE ; j++)
          argv[i][j] = (char)NULL;

      char *pArgv[PROCCTRL_MAX_NARGS];

      unsigned int tmp = 1;

      for (auto &arg : process.m_arguments)
      {
        sprintf( argv[tmp], "%s", arg.c_str());
        pArgv[tmp] = & argv[tmp][0];
        tmp++;
      }

      pArgv[0] = program;
      pArgv[tmp] = NULL;

      char envs[PROCCTRL_MAX_NENV][PROCCTRL_MAX_SIZE];
      char *pEnv[PROCCTRL_MAX_NENV];

      // fills env list
      for (int i = 0; i < PROCCTRL_MAX_NENV ; i++)
        for (int j = 0; j < PROCCTRL_MAX_SIZE ; j++)
          envs[i][j] = (char)NULL;


      tmp = 0;

      // Fills environment list
      for (auto env : process.m_environement)
      {
        std::string var(env.first + "=" + env.second);
        sprintf( envs[tmp], "%s", var.c_str());
        pEnv[tmp] = & envs[tmp][0];
        tmp++;
      }

      pEnv[tmp] = NULL;

      // set new user id to root
      int ret(setuid(0));

      if (ret != 0)
      {
        // Let's try a second time
        ret = setuid(0);

        if (ret != 0)
        {
          std::cout << "child: FATAL couldn't setuid() to 0" << std::endl;
          exit(-1);
        }
      }

      std::stringstream logFile;
      logFile << "/tmp/procctrl" << getpid() << ".log";

      try
      {
        int tmpout = open(
            logFile.str().c_str(),
            O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH
        ); // open file

        dup2( tmpout, 1 );                                         // stdout to file
        dup2( tmpout, 2 );                                         // stderr to file

        close( tmpout );                                       // close unused descriptor
      }
      catch (...)
      {
        std::cout << "child: FATAL couldn't write log file to " << logFile.str() << std::endl;
        exit(-1);
      }

      ret = execve(program, pArgv, pEnv);

      std::cout << "ProcessManager: FATAL, execve came back with ret = " << ret << " , errno = " << errno << std::endl;

      exit(-1);
    }

    //----------------------------------------------------------------------------------

    ProcessStatus ProcessManager::getProcessStatus(
        pid_t pid
    ) const
    {
      std::stringstream procFile;
      procFile << "/proc/" << pid << "/status";

      std::ifstream infile(procFile.str().c_str());

      if (!infile.good())
        return UNDEFINED;

      std::string line;

      while (std::getline(infile, line))
      {
        if (line.substr(0, 6).compare("State:") == 0)
        {
          char status(line.at(6));

          switch(status)
          {
          case 'D':
            return UNINTERUPTIBLE_SLEEP;
          case 'R':
            return RUNNING;
          case 'S':
            return INTERUPTIBLE_SLEEP;
          case 'T':
            return STOPPED;
          case 'W':
            return PAGING;
          case 'X':
            return DEAD;
          case 'Z':
            return ZOMBIE;
          default:
            return UNDEFINED;
          }
        }
      }

      return UNDEFINED;
    }

  }
}

