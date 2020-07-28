// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

class CGameEntityNodeFactory;

struct SEntityScriptProperties
{
	SEntityScriptProperties()
		: pEntityTable(nullptr)
		, pEditorTable(nullptr)
		, pPropertiesTable(nullptr)
		, pInstancePropertiesTable(nullptr)
	{
	}

	IScriptTable* pEntityTable;
	IScriptTable* pEditorTable;
	IScriptTable* pPropertiesTable;
	IScriptTable* pInstancePropertiesTable;
};

class CGameFactory
{
public:
	static void Init(IGameFramework* pGameFramework);

private:
	enum eGameObjectRegistrationFlags
	{
		eGORF_None = 0x0,
		eGORF_InstanceProperties = 0x4,
	};

	static void CreateScriptTables(SEntityScriptProperties& out, uint32 flags);
};
