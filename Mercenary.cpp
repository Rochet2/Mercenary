/*
    _______ _____  _______ _______ _______
    |    ___|     \|    ___|   |   |   |   |
    |    ___|  --  |    ___|       |   |   |
    |_______|_____/|_______|__|_|__|_______|
    �2013-2015 EmuDevs <http://www.emudevs.com/>
    DO NOT REDISTRIBUE! IF I CATCH ANYONE GIVING THIS AWAY, EVERYONE WILL BE REMOVED.
*/
#include "Mercenary.h"
#include "MercenaryMgr.h"

Mercenary::Mercenary() { }

Mercenary::Mercenary(uint32 model, uint8 r, uint8 g, uint8 role, uint8 type)
	: displayId(model), race(r), gender(g), role(role), type(type) { }

Mercenary::~Mercenary()
{
    for (auto itr = ActionContainer.begin(); itr != ActionContainer.end(); ++itr)
        delete &itr;

    for (auto itr = GearContainer.begin(); itr != GearContainer.end(); ++itr)
        delete &itr;

    ActionContainer.clear();
    GearContainer.clear();
}

void Mercenary::LoadActionsFromDB()
{
    ActionContainer.clear();

#ifdef MANGOS
    QueryResult* result = CharacterDatabase.PQuery("SELECT isCast, spellId, castTimer, healPct, castPct FROM mercenary_actions WHERE guid=%u", GetId());
#else
    QueryResult result = CharacterDatabase.PQuery("SELECT isCast, spellId, castTimer, healPct, castPct FROM mercenary_actions WHERE guid=%u", GetId());
#endif
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();

            MercenaryActions actions;
            actions.isCast = fields[0].GetBool();
            actions.spellId = fields[1].GetUInt32();
            actions.castTimer = fields[2].GetUInt32();
            actions.healAtPct = fields[3].GetUInt8();
            actions.castAtPct = fields[4].GetUInt8();

            ActionContainer.push_back(actions);
        } while (result->NextRow());
    }
}

void Mercenary::LoadGearFromDB()
{
    GearContainer.clear();

#ifdef MANGOS
    QueryResult* result = CharacterDatabase.PQuery("SELECT itemId, slot FROM mercenary_gear WHERE guid=%u", GetId());
#else
    QueryResult result = CharacterDatabase.PQuery("SELECT itemId, slot FROM mercenary_gear WHERE guid=%u", GetId());
#endif
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();

            GearContainer.push_back(MercenaryGear(fields[0].GetUInt32(), fields[1].GetUInt8()));
        } while (result->NextRow());
    }
}

#ifdef MANGOS
bool Mercenary::LoadFromDB(QueryResult* result)
#else
bool Mercenary::LoadFromDB(QueryResult result)
#endif
{
    Field* fields = result->Fetch();
    Id = fields[0].GetUInt32();
    ownerGUID = fields[1].GetUInt32();
    role = fields[2].GetUInt8();
    entryId = fields[3].GetUInt32();
    displayId = fields[4].GetUInt32();
    race = fields[5].GetUInt8();
    gender = fields[6].GetUInt8();
    type = fields[7].GetUInt8();
    name = fields[8].GetString();
    level = fields[9].GetUInt8();
    xp = fields[10].GetUInt32();
    minDamage = fields[11].GetFloat();
    maxDamage = fields[12].GetFloat();
    attackTime = fields[13].GetFloat();
    strength = fields[14].GetUInt32();
    agility = fields[15].GetUInt32();
    stamina = fields[16].GetUInt32();
    intellect = fields[17].GetUInt32();
    spirit = fields[18].GetUInt32();
    health = fields[19].GetUInt32();
    mana = fields[20].GetUInt32();
    happiness = fields[21].GetUInt32();
    summoned = fields[22].GetBool();

    LoadActionsFromDB();
    LoadGearFromDB();

    return true;
}

void Mercenary::SaveToDB()
{
#ifndef MANGOS
    SQLTransaction trans = CharacterDatabase.BeginTransaction();

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_MERCENARY);
    stmt->setUInt32(0, Id);
    stmt->setUInt32(1, ownerGUID);
    stmt->setUInt8(2, role);
    stmt->setUInt32(3, entryId);
    stmt->setUInt32(4, displayId);
    stmt->setUInt8(5, race);
    stmt->setUInt8(6, gender);
    stmt->setUInt8(7, type);
    stmt->setString(8, name);
    stmt->setUInt8(9, level);
    stmt->setUInt32(10, xp);
    stmt->setFloat(11, minDamage);
    stmt->setFloat(12, maxDamage);
    stmt->setUInt32(13, attackTime);
    stmt->setUInt32(14, strength);
    stmt->setUInt32(15, agility);
    stmt->setUInt32(16, stamina);
    stmt->setUInt32(17, intellect);
    stmt->setUInt32(18, spirit);
    stmt->setUInt32(19, health);
    stmt->setUInt32(20, mana);
    stmt->setUInt32(21, happiness);
    stmt->setBool(22, summoned);

    trans->Append(stmt);
    CharacterDatabase.CommitTransaction(trans);
#else
    CharacterDatabase.BeginTransaction();

    static SqlStatementID insMerc;
    SqlStatement saveMerc = CharacterDatabase.CreateStatement(insMerc, "INSERT INTO mercenaries (Id, ownerGUID, role, entryId, displayId, race, gender, type, "
        "name, level, xp, minDamage, maxDamage, attackTime, strength, agility, stamina, intellect, spirit, health, mana, happiness, summoned) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    saveMerc.addUInt32(Id);
    saveMerc.addUInt32(ownerGUID);
    saveMerc.addUInt8(role);
    saveMerc.addUInt32(entryId);
    saveMerc.addUInt32(displayId);
    saveMerc.addUInt8(race);
    saveMerc.addUInt8(gender);
    saveMerc.addUInt8(type);
    saveMerc.addString(name);
    saveMerc.addUInt8(level);
    saveMerc.addUInt32(xp);
    saveMerc.addFloat(minDamage);
    saveMerc.addFloat(maxDamage);
    saveMerc.addUInt32(attackTime);
    saveMerc.addUInt32(strength);
    saveMerc.addUInt32(agility);
    saveMerc.addUInt32(stamina);
    saveMerc.addUInt32(intellect);
    saveMerc.addUInt32(spirit);
    saveMerc.addUInt32(health);
    saveMerc.addUInt32(mana);
    saveMerc.addUInt32(happiness);
    saveMerc.addBool(summoned);

    saveMerc.Execute();
    CharacterDatabase.CommitTransaction();
#endif
}

void Mercenary::SaveGearToDB()
{
#ifndef MANGOS
    SQLTransaction trans = CharacterDatabase.BeginTransaction();

    for (auto& itr = GearContainer.begin(); itr != GearContainer.end(); ++itr)
    {
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_MERCENARY_GEAR);
        stmt->setUInt32(0, GetId());
        stmt->setUInt32(1, itr->itemId);
        stmt->setUInt8(2, itr->slot);
        trans->Append(stmt);
    }
    CharacterDatabase.CommitTransaction(trans);
#else
    CharacterDatabase.BeginTransaction();

    static SqlStatementID insGear;

    for (auto& itr = GearContainer.begin(); itr != GearContainer.end(); ++itr)
    {
        SqlStatement saveGear = CharacterDatabase.CreateStatement(insGear, "INSERT INTO mercenary_gear (guid, itemId, slot) VALUES (?, ?, ?)");
        saveGear.addUInt32(GetId());
        saveGear.addInt32(itr->itemId);
        saveGear.addUInt8(itr->slot);
        saveGear.Execute();
    }

    CharacterDatabase.CommitTransaction();
#endif
}

bool Mercenary::Create(Player* player, uint32 entry)
{
    if (!player)
        return false;

    if (!entry)
        return false;

#ifndef MANGOS
    uint32 petNumber = sObjectMgr->GeneratePetNumber();
#else
    uint32 petNumber = sObjectMgr.GeneratePetNumber();
#endif
    Id = petNumber;
    ownerGUID = player->GetGUIDLow();
    role = ROLE_NONE;
    entryId = entry;
    displayId = 1;
    race = 0;
    gender = GENDER_NONE;
    type = MERCENARY_TYPE_NONE;
    name = "Mercenary";
    level = player->getLevel();
    xp = 0;
    minDamage = 0;
    maxDamage = 0;
    attackTime = 0;
    strength = 0;
    agility = 0;
    stamina = 0;
    intellect = 0;
    spirit = 0;
    health = 0;
    mana = 0;
    happiness = 0;
    summoned = false;
    beingCreated = true;

    sMercenaryMgr->SaveToList(this);

    return true;
}

bool Mercenary::Create(Player* player, uint32 entry, uint32 model, uint8 r, uint8 g, uint8 mercType, uint8 role)
{
    if (!player)
        return false;

#ifndef MANGOS
    Pet* pet = new Pet(player, SUMMON_PET);
#else
    Pet* pet = new Pet(SUMMON_PET);
#endif
    if (!pet)
        return false;

    if (!entry)
    {
        delete pet;
        return false;
    }

    if (mercType >= MAX_MERCENARY_TYPES || mercType == MERCENARY_TYPE_NONE)
    {
        delete pet;
        return false;
    }

    float x, y, z, o = 0;
#ifndef MANGOS
    player->GetPosition(x, y, z, o);
#else
    player->GetPosition(x, y, z);
#endif

    pet->Relocate(x, y, z, o);
    if (!pet->IsPositionValid())
    {
#ifndef MANGOS
        TC_LOG_ERROR("misc", "Pet (guidlow %d, entry %d) suggested coordinates isn't valid (X: %f Y: %f)", pet->GetGUIDLow(), entry, pet->GetPositionX(), pet->GetPositionY());
#endif
        delete pet;
        return false;
    }

    Map* map = player->GetMap();
#ifndef MANGOS
    uint32 petNumber = sObjectMgr->GeneratePetNumber();
    if (!pet->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_PET), map, player->GetPhaseMask(), entry, petNumber))
#else
    CreatureCreatePos pos(player, player->GetOrientation());
    CreatureInfo const* creatureInfo = ObjectMgr::GetCreatureTemplate(MERCENARY_DEFAULT_ENTRY);
    if (!creatureInfo)
    {
        delete pet;
        return false;
    }

    uint32 petNumber = sObjectMgr.GeneratePetNumber();
    uint32 guid = pos.GetMap()->GenerateLocalLowGuid(HIGHGUID_PET);

    if (!pet->Create(guid, pos, creatureInfo, petNumber))
#endif
    {
        delete pet;
        return false;
    }

    Id = petNumber;
    ownerGUID = player->GetGUIDLow();
    role = role;
    entryId = entry;
    displayId = model;
    race = r;
    gender = g;
    type = mercType;

#ifndef MANGOS
    pet->SetCreatorGUID(player->GetGUID());
    pet->setPowerType(POWER_MANA);
#else
    pet->SetOwnerGuid(player->GetObjectGuid());
    pet->SetPowerType(POWER_MANA);
    pet->setPetType(SUMMON_PET);
#endif
    pet->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, player->getFaction());
    pet->SetUInt32Value(UNIT_NPC_FLAGS, 1);
    pet->SetName(name);
    pet->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);

    InitStats(player, pet);

#ifdef MANGOS
    pet->SetCreatorGuid(player->GetObjectGuid());
#endif
    pet->GetCharmInfo()->SetPetNumber(petNumber, true);
    pet->SetUInt32Value(UNIT_FIELD_BYTES_0, 2048);
    pet->SetUInt32Value(UNIT_FIELD_PETEXPERIENCE, 0);
    pet->SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, 1000);
    pet->SetDisplayId(model);
    pet->SetPower(POWER_MANA, pet->GetMaxPower(POWER_MANA));
    pet->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, uint32(time(NULL)));

#ifndef MANGOS
    pet->SetFullHealth();
    map->AddToMap(pet->ToCreature());
#else
    pet->SetHealth(pet->GetMaxHealth());
    pet->AIM_Initialize();
    map->Add((Creature*)pet);
#endif

#ifndef MANGOS
    player->SetMinion(pet, true);
#else
    player->SetPet(pet);
#endif
    pet->InitPetCreateSpells();
    pet->InitTalentForLevel();
    player->PetSpellInitialize();

    ChatHandler(player->GetSession()).SendSysMessage("Successfully created a mercenary!");
#ifndef MANGOS
    pet->Say("Thanks for choosing me as your mercenary! If you need help or if you want to change what I do, talk to me.", LANG_UNIVERSAL, player);
    pet->Say("Don't forget to setup my skills, actions and gear!", LANG_UNIVERSAL, player);
#else
    pet->MonsterSay("Thanks for choosing me as your mercenary! If you need help or if you want to change what I do, talk to me.", LANG_UNIVERSAL, player);
    pet->MonsterSay("Don't forget to setup my skills, actions and gear!", LANG_UNIVERSAL, player);
#endif
    name = pet->GetName();
    level = pet->getLevel();
    xp = pet->GetUInt32Value(UNIT_FIELD_PETEXPERIENCE);
    minDamage = pet->GetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE);
    maxDamage = pet->GetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE);
    attackTime = pet->GetAttackTime(BASE_ATTACK);
    strength = pet->GetStat(STAT_STRENGTH);
    agility = pet->GetStat(STAT_AGILITY);
    stamina = pet->GetStat(STAT_STAMINA);
    intellect = pet->GetStat(STAT_INTELLECT);
    spirit = pet->GetStat(STAT_SPIRIT);
    health = pet->GetMaxHealth();
    mana = pet->GetMaxPower(POWER_MANA);
    happiness = 0;
    selectedSpellId = 0;
    selectedCastTime = 0;
    selectedHealPct = 0;
    selectedCastPct = 0;
    summoned = true;
    beingCreated = false;

    for (auto& itr = sMercenaryMgr->MercenaryStartGearBegin(); itr != sMercenaryMgr->MercenaryStartGearEnd(); ++itr)
    {
        if (GetType() == itr->first && role == itr->second.mercenaryRole)
        {
            if (IsValidGear(itr->second.headEntry) && IsValidGear(itr->second.shoulderEntry) && IsValidGear(itr->second.chestEntry)
                && IsValidGear(itr->second.legEntry) && IsValidGear(itr->second.feetEntry) && IsValidGear(itr->second.handEntry))
            {
                GearContainer.push_back(MercenaryGear(itr->second.headEntry, SLOT_HEAD));
                GearContainer.push_back(MercenaryGear(itr->second.shoulderEntry, SLOT_SHOULDERS));
                GearContainer.push_back(MercenaryGear(itr->second.chestEntry, SLOT_CHEST));
                GearContainer.push_back(MercenaryGear(itr->second.legEntry, SLOT_LEGS));
                GearContainer.push_back(MercenaryGear(itr->second.feetEntry, SLOT_FEET));
                GearContainer.push_back(MercenaryGear(itr->second.handEntry, SLOT_HANDS));
                GearContainer.push_back(MercenaryGear(itr->second.weaponEntry, SLOT_MAIN_HAND));
                GearContainer.push_back(MercenaryGear(itr->second.offHandEntry, SLOT_OFF_HAND));
                GearContainer.push_back(MercenaryGear(itr->second.rangedEntry, SLOT_RANGED));
                pet->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, itr->second.weaponEntry);
                pet->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, itr->second.offHandEntry);
                pet->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, itr->second.rangedEntry);
                SaveGearToDB();
            }
            else
#ifndef MANGOS
                TC_LOG_ERROR("misc", "Invalid gear type for mercenary type %u and role %u. Could not load starter gear for Mercenary %u (owner: %u)", 
                GetType(), GetRole(), GetId(), GetOwnerGUID());
#else
                sLog.outError("Invalid gear type for mercenary type %u and role %u. Could not load starter gear for Mercenary %u (owner: %u)",
                GetType(), GetRole(), GetId(), GetOwnerGUID());
#endif
        }
    }

    SaveToDB();
    sMercenaryMgr->SaveToList(this);

    return true;
}

bool Mercenary::CreateAction(Player* player)
{
    WorldSession* session = player->GetSession();
    if (HasMaxActions())
    {
        session->SendNotification("Your Mercenary has enough actions!");
        return false;
    }

    if (!IsProcessingAction())
    {
        session->SendNotification("You did not select a spell to setup!");
        return false;
    }

    for (auto& it = ActionBegin(); it != ActionEnd(); ++it)
    {
        if (selectedSpellId == it->spellId)
        {
            session->SendNotification("Your Mercenary already knows this!");
            return false;
        }
    }

    MercenaryActions action = MercenaryActions();
    action.guid = GetId();
    action.ownerGUID = GetOwnerGUID();
    action.spellId = selectedSpellId;
    action.castTimer = selectedCastTime;
    action.healAtPct = selectedHealPct;
    action.castAtPct = selectedCastPct;
    action.isCast = false;

#ifndef MANGOS
    SQLTransaction trans = CharacterDatabase.BeginTransaction();

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_MERCENARY_ACTION);
    stmt->setUInt32(0, GetId());
    stmt->setUInt32(1, GetOwnerGUID());
    stmt->setBool(2, action.isCast);
    stmt->setUInt32(3, action.spellId);
    stmt->setUInt32(4, action.castTimer);
    stmt->setUInt8(5, action.healAtPct);
    stmt->setInt8(6, action.castAtPct);
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);
#else
    CharacterDatabase.BeginTransaction();

    static SqlStatementID insAction;
    SqlStatement saveAction = CharacterDatabase.CreateStatement(insAction, "INSERT INTO mercenary_actions (guid, ownerGUID, isCast, spellId, castTimer, healPct, castPct) VALUES(?, ?, ?, ?, ?, ?, ?)");

    saveAction.addUInt32(GetId());
    saveAction.addUInt32(GetOwnerGUID());
    saveAction.addBool(action.isCast);
    saveAction.addUInt32(action.spellId);
    saveAction.addUInt32(action.castTimer);
    saveAction.addUInt8(action.healAtPct);
    saveAction.addUInt8(action.castAtPct);

    saveAction.Execute();
    CharacterDatabase.CommitTransaction();
#endif
    ActionContainer.push_back(action);
    return true;
}

bool Mercenary::Summon(Player* player, uint32 entry, uint32 model, uint8 mercType)
{
    if (!player)
        return false;

#ifndef MANGOS
    Pet* pet = new Pet(player, SUMMON_PET);
#else
    Pet* pet = new Pet(SUMMON_PET);
#endif
    if (!pet)
        return false;

    if (!entry)
    {
        delete pet;
        return false;
    }

    if (mercType >= MAX_MERCENARY_TYPES || mercType == MERCENARY_TYPE_NONE)
    {
        delete pet;
        return false;
    }

	float x, y, z, o = 0;
#ifndef MANGOS
    player->GetPosition(x, y, z, o);
#else
    player->GetPosition(x, y, z);
#endif

    pet->Relocate(x, y, z, o);
    if (!pet->IsPositionValid())
    {
#ifndef MANGOS
        TC_LOG_ERROR("misc", "Pet (guidlow %d, entry %d) suggested coordinates isn't valid (X: %f Y: %f)", pet->GetGUIDLow(), entry, pet->GetPositionX(), pet->GetPositionY());
#endif
        delete pet;
        return false;
    }

    Map* map = player->GetMap();
#ifndef MANGOS
    uint32 petNumber = sObjectMgr->GeneratePetNumber();
    if (!pet->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_PET), map, player->GetPhaseMask(), entry, petNumber))
#else
    CreatureCreatePos pos(player, player->GetOrientation());
    CreatureInfo const* creatureInfo = ObjectMgr::GetCreatureTemplate(MERCENARY_DEFAULT_ENTRY);
    if (!creatureInfo)
    {
        delete pet;
        return false;
    }

    uint32 guid = pos.GetMap()->GenerateLocalLowGuid(HIGHGUID_PET);
    if (!pet->Create(guid, pos, creatureInfo, GetId()))
#endif
    {
#ifndef MANGOS
        TC_LOG_ERROR("misc", "Entry %u is invalid", entry);
#endif
        delete pet;
        return false;
    }

    selectedSpellId = 0;
    selectedCastTime = 0;
    selectedHealPct = 0;
    selectedCastPct = 0;

#ifndef MANGOS
    pet->SetCreatorGUID(player->GetGUID());
    pet->setPowerType(POWER_MANA);
#else
    pet->SetOwnerGuid(player->GetObjectGuid());
    pet->SetPowerType(POWER_MANA);
    pet->setPetType(SUMMON_PET);
#endif
    pet->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, player->getFaction());
    pet->SetUInt32Value(UNIT_NPC_FLAGS, 1);
    pet->SetName(name);
    pet->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);

    InitStats(player, pet);

#ifdef MANGOS
    pet->SetCreatorGuid(player->GetObjectGuid());
#endif
    pet->GetCharmInfo()->SetPetNumber(GetId(), true);
    pet->SetUInt32Value(UNIT_FIELD_BYTES_0, 2048);
    pet->SetUInt32Value(UNIT_FIELD_PETEXPERIENCE, 0);
    pet->SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, 1000);
    pet->SetDisplayId(model);
    pet->SetPower(POWER_MANA, pet->GetMaxPower(POWER_MANA));
    pet->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, uint32(time(NULL)));

    for (auto& itr = GearBegin(); itr != GearEnd(); ++itr)
    {
        if (itr->slot == SLOT_MAIN_HAND)
            pet->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, itr->itemId);
        if (itr->slot == SLOT_OFF_HAND)
            pet->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, itr->itemId);
        if (itr->slot == SLOT_RANGED)
            pet->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, itr->itemId);
   }

#ifndef MANGOS
    pet->SetFullHealth();
    map->AddToMap(pet->ToCreature());
#else
    pet->SetHealth(pet->GetMaxHealth());
    pet->AIM_Initialize();
    map->Add((Creature*)pet);
#endif

#ifndef MANGOS
    player->SetMinion(pet, true);
#else
    player->SetPet(pet);
#endif

    pet->InitPetCreateSpells();
    pet->InitTalentForLevel();
    player->PetSpellInitialize();

    return true;
}

bool Mercenary::IsValidGear(uint32 entry)
{
    return entry != 0;
}

bool Mercenary::CanEquipItem(Player* player, Item* item)
{
    WorldSession* session = player->GetSession();
    if (!item)
    {
        ChatHandler(session).SendSysMessage("Item not found! Your mercenary could not equip this item.");
        return false;
    }

#ifndef MANGOS
    const ItemTemplate* proto = item->GetTemplate();
#else
    const ItemPrototype* proto = item->GetProto();
#endif
    if (!proto)
    {
        ChatHandler(session).SendSysMessage("Invalid item! Your mercenary could not equip this item.");
#ifndef MANGOS
        TC_LOG_ERROR("misc", "Mercenary tried to equip invalid item %u. Item does not have a template.", item->GetEntry());
#else
        sLog.outError("Mercenary tried to equip invalid item %u.Item does not have a template.", item->GetEntry());
#endif
        return false;
    }

    Pet* pet = player->GetPet();
    if (!pet)
        return false;

    uint32 itemClass = proto->Class;
    uint32 itemSubClass = proto->SubClass;
    uint32 invType = proto->InventoryType;
    bool isCorrectArmor = ((type == MERCENARY_TYPE_WARRIOR || type == MERCENARY_TYPE_PALADIN || type == MERCENARY_TYPE_DK) &&
        itemSubClass == ITEM_SUBCLASS_ARMOR_PLATE) || ((type == MERCENARY_TYPE_HUNTER || type == MERCENARY_TYPE_SHAMAN) &&
        itemSubClass == ITEM_SUBCLASS_ARMOR_MAIL) || ((type == MERCENARY_TYPE_PRIEST || type == MERCENARY_TYPE_MAGE ||
        type == MERCENARY_TYPE_WARLOCK) && itemSubClass == ITEM_SUBCLASS_ARMOR_CLOTH) || ((type == MERCENARY_TYPE_DRUID ||
        type == MERCENARY_TYPE_ROGUE) && itemSubClass == ITEM_SUBCLASS_ARMOR_LEATHER);
    bool isCorrectWeapon = role == ROLE_MELEE_DPS && ((type == MERCENARY_TYPE_WARRIOR || type == MERCENARY_TYPE_PALADIN ||
        role == ROLE_MELEE_DPS && ((type == MERCENARY_TYPE_ROGUE || type == MERCENARY_TYPE_SHAMAN) && invType == INVTYPE_WEAPON ||
        invType == INVTYPE_WEAPONMAINHAND) || type == MERCENARY_TYPE_DK || type == MERCENARY_TYPE_DRUID) && invType == INVTYPE_2HWEAPON) || role == ROLE_TANK &&
        ((type == MERCENARY_TYPE_WARRIOR || type == MERCENARY_TYPE_PALADIN || type == MERCENARY_TYPE_DK) && invType == INVTYPE_WEAPON || invType == INVTYPE_WEAPONMAINHAND ||
        invType == INVTYPE_SHIELD) || role == ROLE_TANK && ((type == MERCENARY_TYPE_DRUID) && invType == INVTYPE_2HWEAPON) || role == ROLE_CASTER_DPS &&
        ((type == MERCENARY_TYPE_PRIEST || type == MERCENARY_TYPE_WARLOCK || type == MERCENARY_TYPE_MAGE || type == MERCENARY_TYPE_DRUID) && invType == INVTYPE_WEAPON ||
        invType == INVTYPE_HOLDABLE || invType == INVTYPE_WEAPONMAINHAND || (invType == INVTYPE_2HWEAPON && itemSubClass == ITEM_SUBCLASS_WEAPON_STAFF)) || (role == ROLE_CASTER_DPS ||
        role == ROLE_HEALER) && ((type == MERCENARY_TYPE_SHAMAN || type == MERCENARY_TYPE_PALADIN) && invType == INVTYPE_WEAPON || invType == INVTYPE_WEAPONMAINHAND ||
        invType == INVTYPE_SHIELD) || role == ROLE_HEALER && ((type == MERCENARY_TYPE_PALADIN || type == MERCENARY_TYPE_SHAMAN) && invType == INVTYPE_WEAPON || invType == INVTYPE_SHIELD ||
        invType == INVTYPE_WEAPONMAINHAND) || role == ROLE_HEALER && ((type == MERCENARY_TYPE_DRUID || type == MERCENARY_TYPE_PRIEST) && invType == INVTYPE_WEAPON ||
        (invType == INVTYPE_2HWEAPON && itemSubClass == ITEM_SUBCLASS_WEAPON_STAFF) || invType == INVTYPE_HOLDABLE) || role == ROLE_MARKSMAN_DPS && ((type == MERCENARY_TYPE_HUNTER) &&
        invType == INVTYPE_WEAPON || invType == INVTYPE_WEAPONMAINHAND || invType == INVTYPE_RANGED || (invType == INVTYPE_2HWEAPON && (itemSubClass == ITEM_SUBCLASS_WEAPON_POLEARM ||
        itemSubClass == ITEM_SUBCLASS_WEAPON_SPEAR)));
    bool isCorrectLevel = proto->RequiredLevel < level;
    if (proto->RequiredLevel > 0 && isCorrectLevel)
    {
        ChatHandler(session).PSendSysMessage("Equip item failed! Item level is too high. You can equip this item to a Mercenary when they are level %u.", proto->RequiredLevel);
        return false;
    }

    if (itemClass == ITEM_CLASS_ARMOR && (invType != INVTYPE_SHIELD && invType != INVTYPE_HOLDABLE) && !isCorrectArmor)
    {
        ChatHandler(session).SendSysMessage("Equip item failed! Your Mercenary cannot equip this armor type.");
        return false;
    }

    if ((itemClass == ITEM_CLASS_WEAPON || (itemClass == ITEM_CLASS_ARMOR && (invType == INVTYPE_SHIELD || invType == INVTYPE_HOLDABLE))) && !isCorrectWeapon)
    {
        ChatHandler(session).SendSysMessage("Equip item failed! Your Mercenary cannot equip this weapon type. Make sure your Mercenary's role can use this weapon.");
        return false;
    }

    if (editSlot == SLOT_MAIN_HAND)
    {
        if (invType != INVTYPE_2HWEAPON && invType != INVTYPE_WEAPONMAINHAND && invType != INVTYPE_WEAPON)
        {
            ChatHandler(session).SendSysMessage("Equip item failed! Cannot equip an off hand weapon in your Mercenary's main hand.");
            return false;
        }
    }
    else if (editSlot == SLOT_OFF_HAND)
    {
        if (invType == INVTYPE_2HWEAPON && invType == INVTYPE_WEAPONMAINHAND && invType == INVTYPE_WEAPON)
        {
            ChatHandler(session).SendSysMessage("Equip item failed! Cannot equip a two handed, main hand or one handed in your Mercenary's off hand.");
            return false;
        }
    }

    for (auto& itr = GearContainer.begin(); itr != GearContainer.end(); ++itr)
    {
        if (itemClass == ITEM_CLASS_ARMOR)
        {
            if (itr->slot == editSlot)
                itr->itemId = item->GetEntry();
        }
        else if (itemClass == ITEM_CLASS_WEAPON)
        {
            if (itr->slot == editSlot)
                itr->itemId = item->GetEntry();

            if (itr->slot == editSlot && editSlot == SLOT_MAIN_HAND)
                pet->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, itr->itemId);
            else if (itr->slot == editSlot && editSlot == SLOT_OFF_HAND)
                pet->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, itr->itemId);
            else if (itr->slot == editSlot && editSlot == SLOT_RANGED)
                pet->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, itr->itemId);
        }
    }

    if (editSlot != SLOT_MAIN_HAND || editSlot != SLOT_OFF_HAND || editSlot != SLOT_RANGED)
        SendMirrorImagePacket(pet);

    return true;
}

bool Mercenary::InitStats(Player* player, Pet* pet)
{
    uint8 mercenaryLevel = player->getLevel();

#ifndef MANGOS
    CreatureTemplate const* creatureInfo = pet->GetCreatureTemplate();
#else
    CreatureInfo const* creatureInfo = pet->GetCreatureInfo();
#endif
    if (!creatureInfo)
    {
#ifdef MANGOS
        sLog.outError("Could not load creature info for Mercenary");
#else
        TC_LOG_ERROR("misc", "Cloud not load creature info for Mercenary");
#endif
        return false;
    }

    pet->SetLevel(mercenaryLevel);

#ifndef MANGOS
    pet->SetMeleeDamageSchool(SpellSchools(creatureInfo->dmgschool));
#else
    pet->SetMeleeDamageSchool(SpellSchools(creatureInfo->DamageSchool));
#endif
    //@TODO Below, Change by stats (gear)
    pet->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, float(mercenaryLevel * 50));
    pet->SetAttackTime(BASE_ATTACK, BASE_ATTACK_TIME);
    pet->SetAttackTime(OFF_ATTACK, BASE_ATTACK_TIME);
    pet->SetAttackTime(RANGED_ATTACK, BASE_ATTACK_TIME);
    pet->SetFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);

    pet->SetObjectScale(1.0f);

#ifndef MANGOS
    for (uint8 i = SPELL_SCHOOL_HOLY; i < MAX_SPELL_SCHOOL; ++i)
        pet->SetModifierValue(UnitMods(UNIT_MOD_RESISTANCE_START + i), BASE_VALUE, float(creatureInfo->resistance[i]));
#else
    int32 createResistance[MAX_SPELL_SCHOOL] = { 0, 0, 0, 0, 0, 0, 0 };
    createResistance[SPELL_SCHOOL_HOLY] = creatureInfo->ResistanceHoly;
    createResistance[SPELL_SCHOOL_FIRE] = creatureInfo->ResistanceFire;
    createResistance[SPELL_SCHOOL_NATURE] = creatureInfo->ResistanceNature;
    createResistance[SPELL_SCHOOL_FROST] = creatureInfo->ResistanceFrost;
    createResistance[SPELL_SCHOOL_SHADOW] = creatureInfo->ResistanceShadow;
    createResistance[SPELL_SCHOOL_ARCANE] = creatureInfo->ResistanceArcane;
    for (uint8 i = SPELL_SCHOOL_HOLY; i < MAX_SPELL_SCHOOL; ++i)
        pet->SetModifierValue(UnitMods(UNIT_MOD_RESISTANCE_START + i), BASE_VALUE, float(createResistance[i]));
#endif

#ifndef MANGOS
    PetLevelInfo const* petInfo = sObjectMgr->GetPetLevelInfo(creatureInfo->Entry, mercenaryLevel);
#else
    PetLevelInfo const* petInfo = sObjectMgr.GetPetLevelInfo(creatureInfo->Entry, mercenaryLevel);
#endif
    if (petInfo)
    {
        pet->SetCreateHealth(petInfo->health);
        pet->SetCreateMana(petInfo->mana);

        if (petInfo->armor > 0)
            pet->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, float(petInfo->armor));

        for (uint8 stat = 0; stat < MAX_STATS; ++stat)
            pet->SetCreateStat(Stats(stat), float(petInfo->stats[stat]));
    }
    else
    {
#ifndef MANGOS
        CreatureBaseStats const* stats = sObjectMgr->GetCreatureBaseStats(mercenaryLevel, creatureInfo->unit_class);
        pet->SetCreateHealth(stats->BaseHealth[creatureInfo->expansion]);
        pet->SetCreateMana(stats->BaseMana);
#else
        pet->SetCreateHealth(uint32(((float(creatureInfo->MaxLevelHealth) / creatureInfo->MaxLevel) / (1 + 2 * creatureInfo->Rank)) * mercenaryLevel));
        pet->SetCreateMana(uint32(((float(creatureInfo->MaxLevelMana) / creatureInfo->MaxLevel) / (1 + 2 * creatureInfo->Rank)) * mercenaryLevel));
#endif

        pet->SetCreateStat(STAT_STRENGTH, 22);
        pet->SetCreateStat(STAT_AGILITY, 22);
        pet->SetCreateStat(STAT_STAMINA, 25);
        pet->SetCreateStat(STAT_INTELLECT, 28);
        pet->SetCreateStat(STAT_SPIRIT, 27);
    }

#ifndef MANGOS
    pet->SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, uint32(sObjectMgr->GetXPForLevel(mercenaryLevel) * 0.05f));
#else
    pet->SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, uint32(sObjectMgr.GetXPForLevel(mercenaryLevel) * 0.05f));
#endif

    pet->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, float(mercenaryLevel - (mercenaryLevel / 4)));
    pet->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, float(mercenaryLevel + (mercenaryLevel / 4)));

    for (uint8 i = STAT_STRENGTH; i < MAX_STATS; ++i)
        UpdateStats(player, Stats(i), pet);
    for (uint8 i = POWER_MANA; i < MAX_POWERS; ++i)
        UpdateMaxPower(Powers(i), pet);
#ifndef MANGOS
    pet->UpdateAllResistances();
    pet->SetFullHealth();
#else
    for (int i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
        pet->UpdateResistances(i);
    pet->SetHealth(pet->GetMaxHealth());
#endif
    pet->SetPower(POWER_MANA, pet->GetMaxPower(POWER_MANA));

    return true;
}

bool Mercenary::UpdateStats(Player* player, Stats stat, Pet* pet)
{
    if (stat >= MAX_STATS)
        return false;

    float value = pet->GetTotalStatValue(stat);
    pet->ApplyStatBuffMod(stat, player->GetStat(stat), false);
    float ownersBonus = 0.0f;

    float mod = 0.75f;
    if (stat == STAT_STAMINA)
    {
        // Small stamina gain (Basic advantage of having a Mercenary)
#ifndef MANGOS
        value += CalculatePct(player->GetStat(stat), 15);
#else
        value += float(player->GetStat(stat)) * 0.3f;
#endif
    }
    else if (stat == STAT_INTELLECT)
    {
        // Small intellect gain (Basic advantage of having a Mercenary)
#ifndef MANGOS
        value += CalculatePct(player->GetStat(stat), 15);
#else
        value += float(player->GetStat(stat)) * 0.3f;
#endif
    }
    else if (stat == STAT_STRENGTH)
    {
        // Small strength gain (Basic advantage of having a Mercenary)
#ifndef MANGOS
        value += CalculatePct(player->GetStat(stat), 15);
#else
        value += float(player->GetStat(stat)) * 0.3f;
#endif
    }

    for (auto& itr = GearBegin(); itr != GearEnd(); ++itr)
    {
#ifndef MANGOS
        const ItemTemplate* proto = sObjectMgr->GetItemTemplate(itr->itemId);
#else
        const ItemPrototype* proto = sObjectMgr.GetItemPrototype(itr->itemId);
#endif
        if (proto)
        {
            for (uint32 i = 0; i < proto->StatsCount; i++)
            {
                uint32 statType = proto->ItemStat[i].ItemStatType;
                uint32 statTypeVal = proto->ItemStat[i].ItemStatValue;
                if ((stat == STAT_STRENGTH && statType == ITEM_MOD_STRENGTH) || (stat == STAT_AGILITY && statType == ITEM_MOD_AGILITY) ||
                    (stat == STAT_INTELLECT && statType == ITEM_MOD_INTELLECT) || (stat == STAT_STAMINA && statType == ITEM_MOD_STAMINA))
                    value += (float)statTypeVal;
            }
        }
    }

    pet->SetStat(stat, int32(value));
    pet->ApplyStatBuffMod(stat, player->GetStat(stat) + ownersBonus, true);

    switch (stat)
    {
        case STAT_STRENGTH: UpdateStrength(pet); break;
        case STAT_AGILITY: UpdateArmor(pet); break;
        case STAT_STAMINA: UpdateMaxHealth(pet); break;
        case STAT_INTELLECT: UpdateMaxPower(POWER_MANA, pet); break;
        case STAT_SPIRIT:
        default:
            break;
    }

    return true;
}

void Mercenary::SetValues(uint32 model, uint8 r, uint8 g)
{
    displayId = model;
    race = r;
    gender = g;
}

void Mercenary::UpdateGear()
{
#ifndef MANGOS
    SQLTransaction trans = CharacterDatabase.BeginTransaction();

    for (auto& itr = GearContainer.begin(); itr != GearContainer.end(); ++itr)
    {
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_MERCENARY_GEAR);
        stmt->setUInt32(0, itr->itemId);
        stmt->setUInt32(1, GetId());
        stmt->setUInt32(2, itr->slot);
        trans->Append(stmt);
    }

    CharacterDatabase.CommitTransaction(trans);
#else
    CharacterDatabase.BeginTransaction();

    for (auto& itr = GearContainer.begin(); itr != GearContainer.end(); ++itr)
        CharacterDatabase.PExecute("UPDATE mercenary_gear SET itemId=%u WHERE guid=%u AND slot=%u", itr->itemId, GetId(), itr->slot);

    CharacterDatabase.CommitTransaction();
#endif
}

void Mercenary::RemoveOffHand(Creature* creature)
{
    if (!creature)
        return;

    for (auto& itr = GearContainer.begin(); itr != GearContainer.end(); ++itr)
    {
        if (itr->slot == SLOT_OFF_HAND)
            itr->itemId = 0;
    }
    creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, 0);
}

void Mercenary::SendMirrorImagePacket(Creature* creature)
{
    WorldPacket data(SMSG_MIRRORIMAGE_DATA, 68);
#ifndef MANGOS
    data << uint64(creature->GetGUID());
#else
    data << creature->GetObjectGuid();
#endif
    data << uint32(GetDisplay());
    data << uint8(GetRace());
    data << uint8(GetGender());
    data << uint8(1);
    data << uint8(0); // Skin
    data << uint8(0); // Face
    data << uint8(0); // Hair
    data << uint8(0); // Hair color
    data << uint8(0); // Facial hair
    data << uint32(0);
    data << uint32(sMercenaryMgr->GetItemDisplayId(GetItemBySlot(SLOT_HEAD)));
    data << uint32(sMercenaryMgr->GetItemDisplayId(GetItemBySlot(SLOT_SHOULDERS)));
    data << uint32(0); // Shirt?
    data << uint32(sMercenaryMgr->GetItemDisplayId(GetItemBySlot(SLOT_CHEST)));
    data << uint32(0); // Waist
    data << uint32(sMercenaryMgr->GetItemDisplayId(GetItemBySlot(SLOT_LEGS)));
    data << uint32(sMercenaryMgr->GetItemDisplayId(GetItemBySlot(SLOT_FEET)));
    data << uint32(0); // Wrists
    data << uint32(sMercenaryMgr->GetItemDisplayId(GetItemBySlot(SLOT_HANDS)));
    data << uint32(0); // Cloak
    data << uint32(0); // Tabard

    creature->SendMessageToSet(&data, false);
}

void Mercenary::UpdateStrength(Pet* pet, bool ranged)
{
    if (ranged)
        return;

    float val = 0.0f;
    float bonusAP = 0.0f;
    UnitMods unitMod = UNIT_MOD_ATTACK_POWER;

	val = 2 * pet->GetStat(STAT_STRENGTH) - 20.0f;

    Unit* owner = pet->GetOwner();

    pet->SetModifierValue(UNIT_MOD_ATTACK_POWER, BASE_VALUE, val + bonusAP);

    float base_attPower = pet->GetModifierValue(unitMod, BASE_VALUE) * pet->GetModifierValue(unitMod, BASE_PCT);
    float attPowerMod = pet->GetModifierValue(unitMod, TOTAL_VALUE);
    float attPowerMultiplier = pet->GetModifierValue(unitMod, TOTAL_PCT) - 1.0f;

    pet->SetInt32Value(UNIT_FIELD_ATTACK_POWER, (int32)base_attPower);
    pet->SetInt32Value(UNIT_FIELD_ATTACK_POWER_MODS, (int32)attPowerMod);
    pet->SetFloatValue(UNIT_FIELD_ATTACK_POWER_MULTIPLIER, attPowerMultiplier);

    UpdatePhysicalDamage(BASE_ATTACK, pet);
}

void Mercenary::UpdatePhysicalDamage(WeaponAttackType attackType, Pet* pet)
{
    if (attackType > BASE_ATTACK)
        return;

    UnitMods unitMod = UNIT_MOD_DAMAGE_MAINHAND;

    float att_speed = float(pet->GetAttackTime(BASE_ATTACK)) / 1000.0f;
    float base_value = pet->GetModifierValue(unitMod, BASE_VALUE) + pet->GetTotalAttackPowerValue(attackType) / 14.0f * att_speed;
    float base_pct = pet->GetModifierValue(unitMod, BASE_PCT);
    float total_value = pet->GetModifierValue(unitMod, TOTAL_VALUE);
    float total_pct = pet->GetModifierValue(unitMod, TOTAL_PCT);
    float weapon_mindamage = pet->GetWeaponDamageRange(BASE_ATTACK, MINDAMAGE);
    float weapon_maxdamage = pet->GetWeaponDamageRange(BASE_ATTACK, MAXDAMAGE);
    float mindamage = ((base_value + weapon_mindamage) * base_pct + total_value) * total_pct;
    float maxdamage = ((base_value + weapon_maxdamage) * base_pct + total_value) * total_pct;
    for (auto& itr = GearBegin(); itr != GearEnd(); ++itr)
    {
        if (itr->slot == SLOT_MAIN_HAND || itr->slot == SLOT_OFF_HAND)
        {
#ifndef MANGOS
            const ItemTemplate* proto = sObjectMgr->GetItemTemplate(itr->itemId);
#else
            const ItemPrototype* proto = sObjectMgr.GetItemPrototype(itr->itemId);
#endif
            if (proto)
            {
                mindamage += proto->Damage[1].DamageMin;
                maxdamage += proto->Damage[0].DamageMax;
            }
        }
    }

    pet->SetStatFloatValue(UNIT_FIELD_MINDAMAGE, mindamage);
    pet->SetStatFloatValue(UNIT_FIELD_MAXDAMAGE, maxdamage);
}

void Mercenary::UpdateArmor(Pet* pet)
{
    uint32 armor = 0.0f;
    float value = 0.0f;
    float bonusArmor = 0.0f;
    UnitMods unitMod = UNIT_MOD_ARMOR;

    for (auto& itr = GearBegin(); itr != GearEnd(); ++itr)
    {
#ifndef MANGOS
        const ItemTemplate* proto = sObjectMgr->GetItemTemplate(itr->itemId);
#else
        const ItemPrototype* proto = sObjectMgr.GetItemPrototype(itr->itemId);
#endif
        if (proto)
        {
            if (proto->Armor > 0)
                armor += proto->Armor;
        }
    }

    value = pet->GetModifierValue(unitMod, BASE_VALUE);
    value *= pet->GetModifierValue(unitMod, BASE_PCT);
    value += pet->GetStat(STAT_AGILITY) * 1.5f; // Decreased gain by 0.5
    value += pet->GetModifierValue(unitMod, TOTAL_VALUE);
    value *= pet->GetModifierValue(unitMod, TOTAL_PCT);
    value += (float)armor;

    pet->SetArmor(int32(value));
}

void Mercenary::UpdateMaxHealth(Pet* pet)
{
    UnitMods unitMod = UNIT_MOD_HEALTH;
    float stamina = pet->GetStat(STAT_STAMINA) - pet->GetCreateStat(STAT_STAMINA);

    float value = pet->GetModifierValue(unitMod, BASE_VALUE) + pet->GetCreateHealth();
    value *= pet->GetModifierValue(unitMod, BASE_PCT);
    value += pet->GetModifierValue(unitMod, TOTAL_VALUE) + stamina * 10.0f;
    value *= pet->GetModifierValue(unitMod, TOTAL_PCT);

    pet->SetMaxHealth((uint32)value);
}

void Mercenary::UpdateMaxPower(Powers power, Pet* pet)
{
    UnitMods unitMod = UnitMods(UNIT_MOD_POWER_START + power);

    float addValue = (power == POWER_MANA) ? pet->GetStat(STAT_INTELLECT) - pet->GetCreateStat(STAT_INTELLECT) : 0.0f;

    float value = pet->GetModifierValue(unitMod, BASE_VALUE) + pet->GetCreatePowers(power);
    value *= pet->GetModifierValue(unitMod, BASE_PCT);
    value += pet->GetModifierValue(unitMod, TOTAL_VALUE) + addValue * 15.0f;
    value *= pet->GetModifierValue(unitMod, TOTAL_PCT);

    pet->SetMaxPower(power, uint32(value));
}

uint8 Mercenary::GetInvTypeSlot(uint8 characterSlot) const
{
    static uint8 invSlots[] =
    {
        INVTYPE_HEAD,
        INVTYPE_NECK,
        INVTYPE_SHOULDERS,
        NULL,
        INVTYPE_CHEST,
        INVTYPE_WAIST,
        INVTYPE_LEGS,
        INVTYPE_FEET,
        INVTYPE_WRISTS,
        INVTYPE_HANDS,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        INVTYPE_WEAPONMAINHAND,
        INVTYPE_WEAPONOFFHAND,
        INVTYPE_RANGED
    };
    return invSlots[characterSlot];
}

std::string Mercenary::GetSpellIcon(uint32 entry, WorldSession* session) const
{
    std::ostringstream ss;
    ss << "|T";
#ifndef MANGOS
    const SpellInfo* temp = sSpellMgr->GetSpellInfo(entry);
#else
    const SpellEntry* temp = sSpellStore.LookupEntry(entry);
#endif
    const SpellIconEntry* icon = nullptr;
    std::string name = "";
    if (temp)
    {
        name = temp->SpellName[session->GetSessionDbcLocale()];
        icon = sSpellIconStore.LookupEntry(temp->SpellIconID);
        if (icon)
            ss << icon->spellIcon;
    }
    if (!icon)
        ss << "Interface/ICONS/InventoryItems/WoWUnknownItem01";
    ss << ":" << 32 << ":" << 32 << ":" << -18 << ":" << 0 << "|t" << name;
    return ss.str();
}

std::string Mercenary::GetItemIcon(uint32 entry) const
{
    std::ostringstream ss;
    ss << "|TInterface";
#ifndef MANGOS
    const ItemTemplate* temp = sObjectMgr->GetItemTemplate(entry);
#else
    const ItemPrototype* temp = sObjectMgr.GetItemPrototype(entry);
#endif
    const ItemDisplayInfoEntry* dispInfo = nullptr;
    if (temp)
    {
        dispInfo = sItemDisplayInfoStore.LookupEntry(temp->DisplayInfoID);
        if (dispInfo)
            ss << "/ICONS/" << dispInfo->inventoryIcon;
    }
    if (!dispInfo)
        ss << "/InventoryItems/WoWUnknownItem01";
    ss << ":" << 32 << ":" << 32 << ":" << -18 << ":" << 0 << "|t";
    return ss.str();
}

std::string Mercenary::GetSlotIcon(uint8 slot) const
{
    std::ostringstream ss;
    ss << "|TInterface/PaperDoll/";
    switch (slot)
    {
        case SLOT_HEAD: ss << "UI-PaperDoll-Slot-Head"; break;
        case SLOT_SHOULDERS: ss << "UI-PaperDoll-Slot-Shoulder"; break;
        case SLOT_SHIRT: ss << "UI-PaperDoll-Slot-Shirt"; break;
        case SLOT_CHEST: ss << "UI-PaperDoll-Slot-Chest"; break;
        case SLOT_WAIST: ss << "UI-PaperDoll-Slot-Waist"; break;
        case SLOT_LEGS: ss << "UI-PaperDoll-Slot-Legs"; break;
        case SLOT_FEET: ss << "UI-PaperDoll-Slot-Feet"; break;
        case SLOT_WRISTS: ss << "UI-PaperDoll-Slot-Wrists"; break;
        case SLOT_HANDS: ss << "UI-PaperDoll-Slot-Hands"; break;
        case SLOT_BACK: ss << "UI-PaperDoll-Slot-Chest"; break;
        case SLOT_MAIN_HAND: ss << "UI-PaperDoll-Slot-MainHand"; break;
        case SLOT_OFF_HAND: ss << "UI-PaperDoll-Slot-SecondaryHand"; break;
        case SLOT_RANGED: ss << "UI-PaperDoll-Slot-Ranged"; break;
        case SLOT_TABARD: ss << "UI-PaperDoll-Slot-Tabard"; break;
       default: ss << "UI-Backpack-EmptySlot";
    }
    ss << ":" << 32 << ":" << 32 << ":" << -18 << ":" << 0 << "|t";
    return ss.str();
}

const char* Mercenary::GetSlotName(uint8 slot) const
{
    switch (slot)
    {
        case SLOT_HEAD: return  "Head";
        case SLOT_SHOULDERS: return  "Shoulders";
        case SLOT_SHIRT: return  "Shirt";
        case SLOT_CHEST: return  "Chest";
        case SLOT_WAIST: return  "Waist";
        case SLOT_LEGS: return  "Legs";
        case SLOT_FEET: return  "Feet";
        case SLOT_WRISTS: return  "Wrists";
        case SLOT_HANDS: return  "Hands";
        case SLOT_BACK: return  "Back";
        case SLOT_MAIN_HAND: return  "Main hand";
        case SLOT_OFF_HAND: return  "Off hand";
        case SLOT_RANGED: return  "Ranged";
        case SLOT_TABARD: return  "Tabard";
        default: return "";
    }
}

std::string Mercenary::GetItemLink(uint32 entry, WorldSession* session) const
{
#ifndef MANGOS
    const ItemTemplate* temp = sObjectMgr->GetItemTemplate(entry);
#else
    const ItemPrototype* temp = sObjectMgr.GetItemPrototype(entry);
#endif
    int loc_idx = session->GetSessionDbLocaleIndex();
    std::string name = temp->Name1;
#ifndef MANGOS
    if (ItemLocale const* il = sObjectMgr->GetItemLocale(entry))
        ObjectMgr::GetLocaleString(il->Name, loc_idx, name);
#else
    if (ItemLocale const* il = sObjectMgr.GetItemLocale(entry))
        ObjectMgr::GetLocaleString(il->Name, loc_idx, name);
#endif

    std::ostringstream oss;
    oss << "|c" << std::hex << ItemQualityColors[temp->Quality] << std::dec <<
        "|Hitem:" << entry << ":0:0:0:0:0:0:0:0:0|h[" << name << "]|h|r";

    return oss.str();
}