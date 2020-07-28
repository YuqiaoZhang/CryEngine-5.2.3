// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#include "StdAfx.h"
#include "game/GameRules.h"
#include "game/GameFactory.h"
#include "player/Player.h"
#include "player/extensions/ViewExtension.h"
#include "player/extensions/InputExtension.h"
#include "player/extensions/MovementExtension.h"
#include "Entities/GameGeomEntity.h"
#include "Entities/AnimObject.h"

#define HIDE_FROM_EDITOR(className)                                                               \
	{                                                                                             \
		IEntityClass *pItemClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(className); \
		pItemClass->SetFlags(pItemClass->GetFlags() | ECLF_INVISIBLE);                            \
	}

#define REGISTER_GAME_OBJECT(framework, name, script)                                     \
	{                                                                                     \
		IEntityClassRegistry::SEntityClassDesc clsDesc;                                   \
		clsDesc.sName = #name;                                                            \
		clsDesc.sScriptFile = script;                                                     \
		struct C##name##Creator : public IGameObjectExtensionCreatorBase                  \
		{                                                                                 \
			IGameObjectExtensionPtr Create()                                              \
			{                                                                             \
				return ComponentCreate_DeleteWithRelease<C##name>();                      \
			}                                                                             \
			void GetGameObjectExtensionRMIData(void **ppRMI, size_t *nCount)              \
			{                                                                             \
				C##name::GetGameObjectExtensionRMIData(ppRMI, nCount);                    \
			}                                                                             \
		};                                                                                \
		static C##name##Creator _creator;                                                 \
		framework->GetIGameObjectSystem()->RegisterExtension(#name, &_creator, &clsDesc); \
		C##name::SetExtensionId(framework->GetIGameObjectSystem()->GetID(#name));         \
	}

#define REGISTER_NOSCRIPT_GAME_OBJECT(framework, name, editorPath, flags)                       \
	{                                                                                           \
		SEntityScriptProperties props;                                                          \
		CGameFactory::CreateScriptTables(props, flags);                                         \
		gEnv->pScriptSystem->SetGlobalValue(#name, props.pEntityTable);                         \
                                                                                                \
		C##name::RegisterProperties(props);                                                     \
                                                                                                \
		props.pEditorTable->SetValue("EditorPath", editorPath);                                 \
                                                                                                \
		props.pEntityTable->SetValue("Editor", props.pEditorTable);                             \
		props.pEntityTable->SetValue("Properties", props.pPropertiesTable);                     \
                                                                                                \
		if (flags & eGORF_InstanceProperties)                                                   \
		{                                                                                       \
			props.pEntityTable->SetValue("PropertiesInstance", props.pInstancePropertiesTable); \
		}                                                                                       \
                                                                                                \
		IEntityClassRegistry::SEntityClassDesc clsDesc;                                         \
		clsDesc.sName = #name;                                                                  \
		clsDesc.pScriptTable = props.pEntityTable;                                              \
		struct C##name##Creator : public IGameObjectExtensionCreatorBase                        \
		{                                                                                       \
			IGameObjectExtensionPtr Create()                                                    \
			{                                                                                   \
				return ComponentCreate_DeleteWithRelease<C##name>();                            \
			}                                                                                   \
			void GetGameObjectExtensionRMIData(void **ppRMI, size_t *nCount)                    \
			{                                                                                   \
				C##name::GetGameObjectExtensionRMIData(ppRMI, nCount);                          \
			}                                                                                   \
		};                                                                                      \
		static C##name##Creator _creator;                                                       \
		framework->GetIGameObjectSystem()->RegisterExtension(#name, &_creator, &clsDesc);       \
		C##name::SetExtensionId(framework->GetIGameObjectSystem()->GetID(#name));               \
	}

#define REGISTER_GAME_OBJECT_EXTENSION(framework, name)                               \
	{                                                                                 \
		struct C##name##Creator : public IGameObjectExtensionCreatorBase              \
		{                                                                             \
			IGameObjectExtensionPtr Create()                                          \
			{                                                                         \
				return ComponentCreate_DeleteWithRelease<C##name>();                  \
			}                                                                         \
			void GetGameObjectExtensionRMIData(void **ppRMI, size_t *nCount)          \
			{                                                                         \
				C##name::GetGameObjectExtensionRMIData(ppRMI, nCount);                \
			}                                                                         \
		};                                                                            \
		static C##name##Creator _creator;                                             \
		framework->GetIGameObjectSystem()->RegisterExtension(#name, &_creator, NULL); \
		C##name::SetExtensionId(framework->GetIGameObjectSystem()->GetID(#name));     \
	}

void CGameFactory::Init(IGameFramework* pGameFramework)
{
	REGISTER_GAME_OBJECT(pGameFramework, Player, "");
	HIDE_FROM_EDITOR("Player");

	REGISTER_GAME_OBJECT_EXTENSION(pGameFramework, InputExtension);
	REGISTER_GAME_OBJECT_EXTENSION(pGameFramework, MovementExtension);
	REGISTER_GAME_OBJECT_EXTENSION(pGameFramework, ViewExtension);

	REGISTER_GAME_OBJECT(pGameFramework, GameRules, "");
	HIDE_FROM_EDITOR("GameRules");

	REGISTER_NOSCRIPT_GAME_OBJECT(pGameFramework, GameGeomEntity, "Game/GeomEntity", eGORF_None);
	REGISTER_NOSCRIPT_GAME_OBJECT(pGameFramework, AnimObject, "Physics/AnimObject", eGORF_None);

	pGameFramework->GetIGameRulesSystem()->RegisterGameRules("SinglePlayer", "GameRules");
	pGameFramework->GetIGameRulesSystem()->AddGameRulesAlias("SinglePlayer", "sp");
}

void CGameFactory::CreateScriptTables(SEntityScriptProperties& out, uint32 flags)
{
	out.pEntityTable = gEnv->pScriptSystem->CreateTable();
	out.pEntityTable->AddRef();

	out.pEditorTable = gEnv->pScriptSystem->CreateTable();
	out.pEditorTable->AddRef();

	out.pPropertiesTable = gEnv->pScriptSystem->CreateTable();
	out.pPropertiesTable->AddRef();

	if (flags & eGORF_InstanceProperties)
	{
		out.pInstancePropertiesTable = gEnv->pScriptSystem->CreateTable();
		out.pInstancePropertiesTable->AddRef();
	}
}
