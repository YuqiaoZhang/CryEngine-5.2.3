#ifndef _ENTITIES_ANIMOBJECT_H_
#define _ENTITIES_ANIMOBJECT_H_ 1

struct SEntityScriptProperties;

class CAnimObject : public CGameObjectExtensionHelper<CAnimObject, ISimpleExtension>
{
public:
	CAnimObject();
	virtual ~CAnimObject();

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

#endif