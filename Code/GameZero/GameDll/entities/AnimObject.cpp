#include "StdAfx.h"
#include "AnimObject.h"
#include "Game/GameFactory.h"

CAnimObject::CAnimObject()
{
}

CAnimObject::~CAnimObject()
{
}

bool CAnimObject::Init(IGameObject *pGameObject)
{
	SetGameObject(pGameObject);
	return true;
}

void CAnimObject::PostInit(IGameObject *pGameObject)
{
	Reset();
}

void CAnimObject::Release()
{
	ISimpleExtension::Release();
}

bool CAnimObject::RegisterProperties(SEntityScriptProperties &tables)
{
	if (tables.pPropertiesTable)
	{
		tables.pPropertiesTable->SetValue("object_Model", "objects/tutorialcga.cga");
	}

	if (tables.pEditorTable)
	{
		tables.pEditorTable->SetValue("Icon", "prompt.bmp");
		tables.pEditorTable->SetValue("IconOnTop", true);
	}

	return true;
}

void CAnimObject::ProcessEvent(SEntityEvent &event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_EDITOR_PROPERTY_CHANGED:
	case ENTITY_EVENT_RESET:
	{
		Reset();
	}
	break;
	}
}

void CAnimObject::Reset()
{
	SmartScriptTable propertiesTable;
	GetEntity()->GetScriptTable()->GetValue("Properties", propertiesTable);

	const char *geometryPath = "";
	if (propertiesTable->GetValue("object_Model", geometryPath))
	{
		GetEntity()->LoadCharacter(0, geometryPath);
	}
}
