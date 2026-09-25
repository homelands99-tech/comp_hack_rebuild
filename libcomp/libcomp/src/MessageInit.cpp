/**
 * @file libcomp/src/MessageInit.cpp
 * @ingroup libcomp
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Indicates that the server should finish initialization.
 *
 * This file is part of the COMP_hack Library (libcomp).
 *
 * Copyright (C) 2012-2020 COMP_hack Team <compomega@tutanota.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MessageInit.h"

#ifndef EXOTIC_PLATFORM
#include "BaseScriptEngine.h"
#endif  // !EXOTIC_PLATFORM

using namespace libcomp;

Message::Init::Init() {}

Message::Init::~Init() {}

Message::MessageType Message::Init::GetType() const {
  return MessageType::MESSAGE_TYPE_SYSTEM;
}

libcomp::String Message::Init::Dump() const { return "Message: Init"; }

libcomp::Message::Message* libcomp::Message::Init::Clone() const {
  return new libcomp::Message::Init(*this);
}

#ifndef EXOTIC_PLATFORM
namespace libcomp {

template <>
BaseScriptEngine& BaseScriptEngine::Using<Message::Init>() {
  if (!BindingExists("Message.Init", true)) {
    Using<Message::Message>();

    Sqrat::DerivedClass<Message::Init, Message::Message,
                        Sqrat::NoConstructor<Message::Init>>
        binding(mVM, "Message.Init");
    Bind<Message::Init>("Message.Init", binding);
  }

  return *this;
}

}  // namespace libcomp
#endif  // !EXOTIC_PLATFORM
