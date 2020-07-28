// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.


#pragma once


struct SEntityScriptProperties;

class CGameGeomEntity : public CGameObjectExtensionHelper<CGameGeomEntity, ISimpleExtension>
{
public:
	CGameGeomEntity();
	virtual ~CGameGeomEntity();

	//ISimpleExtension
	virtual bool Init(IGameObject* pGameObject) override;
	virtual void PostInit(IGameObject* pGameObject) override;
	virtual void Release() override;
	virtual void ProcessEvent(SEntityEvent& event) override;
	//~ISimpleExtension

	static bool RegisterProperties(SEntityScriptProperties& tables);
	
private:
	void Reset();
};