/**
 * @file libcomp/src/ConnectionMessage.cpp
 * @ingroup libcomp
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Base class for connection messages.
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

#include "ConnectionMessage.h"

#ifndef EXOTIC_PLATFORM
#include "BaseScriptEngine.h"
#endif  // !EXOTIC_PLATFORM

using namespace libcomp;

Message::ConnectionMessage::~ConnectionMessage() {}

Message::MessageType Message::ConnectionMessage::GetType() const {
  return MessageType::MESSAGE_TYPE_CONNECTION;
}

#ifndef EXOTIC_PLATFORM
namespace libcomp {

template <>
BaseScriptEngine& BaseScriptEngine::Using<Message::ConnectionMessage>() {
  if (!BindingExists("Message.ConnectionMessage", true)) {
    Using<Message::Message>();

    Sqrat::DerivedClass<Message::ConnectionMessage, Message::Message,
                        Sqrat::NoConstructor<Message::ConnectionMessage>>
        binding(mVM, "Message.ConnectionMessage");
    Bind<Message::ConnectionMessage>("Message.ConnectionMessage", binding);

    binding
        .Func("GetConnectionMessageType",
              &Message::ConnectionMessage::GetConnectionMessageType)
        .Prop("ConnectionMessageType",
              &Message::ConnectionMessage::GetConnectionMessageType);
  }

  return *this;
}

}  // namespace libcomp
#endif  // !EXOTIC_PLATFORM
