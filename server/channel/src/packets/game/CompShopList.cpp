/**
 * @file server/channel/src/packets/game/CompShopList.cpp
 * @ingroup channel
 *
 * @author HACKfrost
 *
 * @brief Request from the client for the list of COMP shops.
 *
 * This file is part of the Channel Server (channel).
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

#include "Packets.h"

// libcomp Includes
#include <ManagerPacket.h>
#include <Packet.h>
#include <PacketCodes.h>
#include <ServerDataManager.h>

// object Includes
#include <ServerShop.h>

// channel Includes
#include "ChannelClientConnection.h"
#include "ChannelServer.h"
#include "Gacha.h"

using namespace channel;

bool Parsers::CompShopList::Parse(
    libcomp::ManagerPacket* pPacketManager,
    const std::shared_ptr<libcomp::TcpConnection>& connection,
    libcomp::ReadOnlyPacket& p) const {
  if (p.Size() != 4) {
    return false;
  }

  int32_t trendTime = p.ReadS32Little();

  auto server =
      std::dynamic_pointer_cast<ChannelServer>(pPacketManager->GetServer());
  auto serverDataManager = server->GetServerDataManager();

  auto client = std::dynamic_pointer_cast<ChannelClientConnection>(connection);

  // Gacha shops are only listed while they are enabled and their draw tab
  // conditions pass
  std::list<uint32_t> compShopIDs;
  for (uint32_t compShopID : serverDataManager->GetCompShopIDs()) {
    auto shop = serverDataManager->GetShopData(compShopID);
    if (gacha::IsGacha(shop) && !gacha::IsAvailable(server, client, shop)) {
      continue;
    }

    compShopIDs.push_back(compShopID);
  }

  libcomp::Packet reply;
  reply.WritePacketCode(ChannelToClientPacketCode_t::PACKET_COMP_SHOP_LIST);
  reply.WriteS32Little(trendTime);

  if (compShopIDs.size() > 0) {
    reply.WriteS32Little(0);  // First idx

    int8_t idx = 0;
    for (uint32_t compShopID : compShopIDs) {
      auto shop = serverDataManager->GetShopData(compShopID);
      libcomp::String name = shop ? shop->GetName() : "";

      reply.WriteString16Little(libcomp::Convert::ENCODING_DEFAULT,
                                !name.IsEmpty() ? name : "?", true);
      reply.WriteS8(idx++);
      reply.WriteS32Little(0);  // New item flag
      reply.WriteS8(1);         // Enabled
      // Category kind: the client opens the gacha window instead of the
      // normal shop for non-zero values (needs a client that supports it)
      reply.WriteS8(gacha::IsGacha(shop) ? 1 : 0);
      reply.WriteU32Little(compShopID);

      // Notify if more exist
      reply.WriteS32Little((size_t)idx == compShopIDs.size() ? -1
                                                             : (int32_t)idx);
    }
  } else {
    reply.WriteS32Little(-1);  // No first idx, none exist
  }

  connection->SendPacket(reply);

  return true;
}
