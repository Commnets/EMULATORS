/*
 * emudore, Commodore 64 emulator
 * Copyright (c) 2016, Mario Ballano <mballano@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

#include "util.h"
#include "debugger.h"

Debugger::Debugger()
{
}

Debugger::~Debugger()
{
}

std::vector<std::string> Debugger::split_cmd(const std::string &s)
{
  return (std::vector<std::string> ());
}

uint16_t Debugger::emu_seek(uint16_t offset, int whence)
{
  return offset_;
}

uint8_t * Debugger::emu_read_mem(size_t sz)
{
  uint8_t *mem = (uint8_t *) malloc(sz);
  for(size_t i = 0 ; i < sz ; i++)
    mem[i] = mem_->read_byte(offset_ + i);
  return mem;
}

void Debugger::emu_write_mem(uint8_t *mem, size_t sz)
{
  for(size_t i = 0 ; i < sz ; i++)
  {
    mem_->write_byte(offset_++,mem[i]);
  }
}

std::string Debugger::regs_cmd()
{
  std::stringstream v;
  v << std::hex << std::setfill('0');
  v << "a = 0x" << std::setw(2) << (unsigned int) cpu_->a() << "\n";
  v << "x = 0x" << std::setw(2) << (unsigned int) cpu_->x() << "\n";
  v << "y = 0x" << std::setw(2) << (unsigned int) cpu_->y() << "\n";
  v << "sp = 0x" << std::setw(2) << (unsigned int) cpu_->sp() << "\n";
  v << "pc = 0x" << std::setw(4) << (unsigned int) cpu_->pc() << "\n";
  v << "flags = ";
  if(cpu_->cf())  v << "cf ";
  if(cpu_->zf())  v << "zf ";
  if(cpu_->idf()) v << "idf ";
  if(cpu_->dmf()) v << "dmf ";
  if(cpu_->bcf()) v << "bcf ";
  if(cpu_->of())  v << "of ";
  if(cpu_->nf())  v << "nf ";
  v << "\n";
  return v.str();
}

std::string Debugger::emu_handle_cmd(const std::string &s)
{
  std::vector<std::string> args = split_cmd(s);
  std::string r("");
  try
  {
    if(args.at(0) == "dr")
      r = regs_cmd();
  }
  catch(const std::out_of_range){}
  return r;
}

bool Debugger::emulate()
{ 
  return true;
}
