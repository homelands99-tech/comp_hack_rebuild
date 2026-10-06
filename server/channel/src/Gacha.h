/**
 * @file server/channel/src/Gacha.h
 * @ingroup channel
 *
 * @brief Helpers shared by the gacha (ServerShop type GACHA) handlers.
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SERVER_CHANNEL_SRC_GACHA_H
#define SERVER_CHANNEL_SRC_GACHA_H

// object Includes
#include <ServerShop.h>
#include <ServerShopTab.h>

// channel Includes
#include "ChannelClientConnection.h"
#include "ChannelServer.h"
#include "EventManager.h"

namespace channel {

namespace gacha {

/**
 * Name of the tab holding the "draw" product. The client looks the tab up
 * by this name. UTF-8 for "ガチャ" (escaped so the source encoding does not
 * matter).
 */
static const char* const DRAW_TAB_NAME = "\xE3\x82\xAC\xE3\x83\x81\xE3\x83\xA3";

/**
 * Check if the shop is a gacha.
 * @param shop Shop to check
 * @return true if the shop is a gacha
 */
inline bool IsGacha(const std::shared_ptr<objects::ServerShop>& shop) {
  return shop && shop->GetType() == objects::ServerShop::Type_t::GACHA;
}

/**
 * Get the draw tab of a gacha.
 * @param shop Gacha shop
 * @return Draw tab or null if it does not exist
 */
inline std::shared_ptr<objects::ServerShopTab> GetDrawTab(
    const std::shared_ptr<objects::ServerShop>& shop) {
  for (auto tab : shop->GetTabs()) {
    if (tab->GetName() == DRAW_TAB_NAME) {
      return tab;
    }
  }

  return nullptr;
}

/**
 * Check if a tab's conditions (if any) currently pass for the client.
 * @param server Pointer to the channel server
 * @param client Client to evaluate the conditions for
 * @param tab Tab to check
 * @return true if the tab is open
 */
inline bool IsTabOpen(const std::shared_ptr<ChannelServer>& server,
                      const std::shared_ptr<ChannelClientConnection>& client,
                      const std::shared_ptr<objects::ServerShopTab>& tab) {
  if (tab->ConditionsCount() == 0) {
    return true;
  }

  return server->GetEventManager()->EvaluateEventConditions(
      client->GetClientState()->GetZone(), tab->GetConditions(), client);
}

/**
 * Check if a gacha is currently shown and drawable for the client: it must
 * not be disabled and its draw tab conditions must pass.
 * @param server Pointer to the channel server
 * @param client Client to check for
 * @param shop Gacha shop
 * @return true if the gacha is available
 */
inline bool IsAvailable(const std::shared_ptr<ChannelServer>& server,
                        const std::shared_ptr<ChannelClientConnection>& client,
                        const std::shared_ptr<objects::ServerShop>& shop) {
  if (!IsGacha(shop) || shop->GetDisabled()) {
    return false;
  }

  auto drawTab = GetDrawTab(shop);
  return drawTab && IsTabOpen(server, client, drawTab);
}

}  // namespace gacha

}  // namespace channel

#endif  // SERVER_CHANNEL_SRC_GACHA_H
