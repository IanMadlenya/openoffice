/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#include "FileLoggerImpl.hxx"
#include <iostream>

using namespace std;

namespace util
{

    FileLoggerImpl::FileLoggerImpl(const string& fileName) :
        file_(fileName.c_str())
    {
        if (!file_)
            throw "Cannot open file";
    }

    void FileLoggerImpl::beginTree()
    {
        file_ << "digraph {" << endl;
    }

    void FileLoggerImpl::endTree()
    {
        file_ << "}" << endl;
    }

    void FileLoggerImpl::beginNode(const std::string& nodeId, const std::string& value, const std::string& refersToNodeId, bool inUse)
    {
        if (!nodeStack_.empty())
        {
		  if (inUse)
            file_ << nodeId << " [ label=\"(" << value << ")\", shape=box, color=grey, style=filled ];"<< endl;
		  else
			file_ << nodeId << " [ label=\"(" << value << ")\" ];"<< endl;

		  file_ << nodeStack_.top() << " -> " << nodeId << ";" << endl;

		  if (!refersToNodeId.empty())
			file_ << nodeId << " -> " << refersToNodeId << " [ color=grey, weight=0 ];" << endl;
        }
        else
		{
			file_ << nodeId << " [ label=\"(" << value << ")\", shape=diamond ];"<< endl;            
		}
        nodeStack_.push(nodeId);
    }

    void FileLoggerImpl::endNode(const std::string& nodeId)
    {
        nodeStack_.pop();
    }

} // namespace util

