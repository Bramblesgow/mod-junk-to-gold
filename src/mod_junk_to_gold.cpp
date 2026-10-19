#include "Chat.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "QuestDef.h"
#include "ScriptMgr.h"

#include <limits>

class JunkToGold : public PlayerScript
{
public:
    JunkToGold() : PlayerScript("JunkToGold") {}

    /// Converts newly looted gray items only when no accepted quest needs them.
    /// Removes the loot quantity, not the entire inventory stack it was merged into.
    void OnPlayerLootItem(Player* player, Item* item, uint32 count, ObjectGuid /*lootguid*/) override
    {
        if (!player || !item || !item->GetTemplate() || !count)
        {
            return;
        }

        ItemTemplate const* itemTemplate = item->GetTemplate();
        uint32 const itemId = itemTemplate->ItemId;
        if (itemTemplate->Quality != ITEM_QUALITY_POOR ||
            itemTemplate->StartQuest || IsNeededForActiveQuest(player, itemId))
        {
            return;
        }

        // Keep gray loot when the player is at the gold cap, even if it has no value.
        // Positive payouts that reach the cap would be rejected by ModifyMoney.
        uint64 const money = uint64(itemTemplate->SellPrice) * count;
        uint64 const newBalance = uint64(player->GetMoney()) + money;
        // Keep the cap rejection separate so an isolated runtime test can
        // confirm that this path exits before changing inventory or money.
        if (money > static_cast<uint64>(std::numeric_limits<int32>::max()) ||
            newBalance >= MAX_MONEY_AMOUNT)
        {
            LOG_DEBUG("module", "JunkToGold: skipped payout (item {}, count {}, balance {}, payout {}, proposed {})",
                itemId, count, player->GetMoney(), money, newBalance);
            return;
        }

        // HasItemCount excludes stacks marked for trade, as DestroyItemCount does.
        // GetItemCount includes those stacks and could allow payment without removal.
        if (!player->HasItemCount(itemId, count))
        {
            return;
        }

        // StoreNewItem can merge and split loot across stacks. Destroy only the newly
        // looted count; never dereference 'item' after inventory destruction.
        player->DestroyItemCount(itemId, count, true);
        player->ModifyMoney(static_cast<int32>(money));
        SendTransactionInformation(player, itemTemplate, count, static_cast<uint32>(money));
    }

private:
    /// Checks exact item IDs for incomplete and complete (not yet turned-in) quests.
    /// HasQuestForItem's default lookup misses completed objectives after looting.
    static bool IsNeededForActiveQuest(Player const* player, uint32 itemId)
    {
        for (uint16 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
        {
            uint32 const questId = player->GetQuestSlotQuestId(slot);
            if (!questId)
            {
                continue;
            }

            QuestStatus const status = player->GetQuestStatus(questId);
            if (status != QUEST_STATUS_INCOMPLETE && status != QUEST_STATUS_COMPLETE)
            {
                continue;
            }

            Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
            if (!quest)
            {
                continue;
            }

            for (uint8 i = 0; i < QUEST_ITEM_OBJECTIVES_COUNT; ++i)
            {
                if (quest->RequiredItemId[i] == itemId && quest->RequiredItemCount[i] > 0)
                {
                    return true;
                }
            }

            for (uint8 i = 0; i < QUEST_SOURCE_ITEM_IDS_COUNT; ++i)
            {
                if (quest->ItemDrop[i] == itemId)
                {
                    return true;
                }
            }
        }

        return false;
    }

    /// Sends the receipt using the validated payout and stable item template.
    void SendTransactionInformation(Player* player, ItemTemplate const* itemTemplate, uint32 count, uint32 payout)
    {
        std::string name;
        if (count > 1)
        {
            name = Acore::StringFormat("|cff9d9d9d|Hitem:{}::::::::80:::::|h[{}]|h|rx{}", itemTemplate->ItemId, itemTemplate->Name1, count);
        }
        else
        {
            name = Acore::StringFormat("|cff9d9d9d|Hitem:{}::::::::80:::::|h[{}]|h|r", itemTemplate->ItemId, itemTemplate->Name1);
        }

        uint32 const gold = payout / GOLD;
        uint32 const silver = (payout % GOLD) / SILVER;
        uint32 const copper = payout % SILVER;

        std::string info;
        if (payout < SILVER)
        {
            info = Acore::StringFormat("{} sold for {} copper.", name, copper);
        }
        else if (payout < GOLD)
        {
            if (copper > 0)
            {
                info = Acore::StringFormat("{} sold for {} silver and {} copper.", name, silver, copper);
            }
            else
            {
                info = Acore::StringFormat("{} sold for {} silver.", name, silver);
            }
        }
        else
        {
            if (copper > 0 && silver > 0)
            {
                info = Acore::StringFormat("{} sold for {} gold, {} silver and {} copper.", name, gold, silver, copper);
            }
            else if (copper > 0)
            {
                info = Acore::StringFormat("{} sold for {} gold and {} copper.", name, gold, copper);
            }
            else if (silver > 0)
            {
                info = Acore::StringFormat("{} sold for {} gold and {} silver.", name, gold, silver);
            }
            else
            {
                info = Acore::StringFormat("{} sold for {} gold.", name, gold);
            }
        }

        ChatHandler(player->GetSession()).SendSysMessage(info);
    }
};

void Addmod_junk_to_goldScripts()
{
    new JunkToGold();
}
