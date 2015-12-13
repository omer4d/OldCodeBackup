#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <atomic>
#include <map>
#include <functional>
#include <allegro.h>
#include <list>
#include <tuple>
#include <vector>

#include "Buffer.hpp"

BITMAP* buffer;

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
}

void deinit()
{
     destroy_bitmap(buffer);
}


void crash(char const* msg)
{
	printf("%s\n", msg);
	
	int* t = nullptr;
	*t = 0;
}

class Entity;

class Component {
    Entity* owner;
    
    void setOwner(Entity* owner)
    {
        this->owner = owner;
    }
    
public:
    Entity* getOwner() const
    {
        return owner;
    }
    
    virtual char const* getClassName() const = 0;
    
    Component()
    {
        owner = NULL;
    }
    
    virtual ~Component()
    {
    }
    
    friend class Entity;
};

#define COMPONENT_START(X) class X: public Component {           		\
                            public:                              		\
                              static char const* getClassNameStatic()  	\
                              {                                  		\
                                  return #X;                     		\
                              }                                  		\
                                                                 		\
                              char const* getClassName() const   		\
                              {                                  		\
                                  return X::getClassNameStatic();      	\
                              }                                  		\
                                                                 		\
                            private:                             		\
                            
#define COMPONENT_END      };

#define DERIVED_COMPONENT_START(X, Y) class X: Y, public Component {           \
                                       public:                                 \
                                         static char const* getClassType()     \
                                         {                                     \
                                            return #X;                         \
                                         }                                     \
                                                                               \
                                         char const* getType() const           \
                                         {                                     \
                                            return X::getClassType();          \
                                         }                                     \
                                                                               \
                                       private:                                \

#define DERIVED_COMPONENT_END         };

#define CLASSES(...) __VA_ARGS__


class Entity {
	int mId;
	bool mRequestedDeath;
	int mFramesSinceDeathRequest;
	std::function<void(float dt)> mLogicCallback;
	std::map<std::string, Component*> mComponents;
	
	static int generateUniqueId() // Thread-safe!
	{
    	static std::atomic<int> id{0};  
    	return ++id;
	}
	
public:
	Entity()
	{
		mId = generateUniqueId();
		mRequestedDeath = false;
		mFramesSinceDeathRequest = 0;
	}
	
	~Entity()
	{
		clearComponents();
	}
	
	int getId() const
	{
		return mId;
	}
	
	bool getRequestedDeath() const
	{
		return mRequestedDeath;
	}
	
	int getFramesSinceDeathRequest() const
	{
		return mFramesSinceDeathRequest;
	}
	
	void requestDeath()
	{
		mRequestedDeath = true;
	}
	
	void setLogicCallback(std::function<void(float dt)> logicCallback)
	{
		mLogicCallback = logicCallback;
	}
	
	void clearComponents()
	{
		for(auto& kv : mComponents)
			delete kv.second;
		mComponents.clear();
	}
	
	void addComponent(Component* comp)
	{
		if(mComponents.find(comp->getClassName()) != mComponents.end())
			crash("Entity already has this component!");
		
		mComponents[comp->getClassName()] = comp;
	}
	
	template <class T>
	T* getComponent() const
	{
		auto kv = mComponents.find(T::getClassNameStatic());
		return (kv != mComponents.end()) ? (T*)kv->second : nullptr;
	}

	Component* getComponent(std::string const& className)
	{
		auto kv = mComponents.find(className);
		return (kv != mComponents.end()) ? kv->second : nullptr;
	}
	
	void logic(float dt)
	{
		if(mRequestedDeath)
			++mFramesSinceDeathRequest;
		
		if(mLogicCallback)
			mLogicCallback(dt);
	}
};


class System {
public:
	virtual ~System() = 0;
	virtual void addEntity(Entity* ent) = 0;
	virtual void removeEntity(Entity* ent) = 0;
	virtual bool caresAbout(Entity* ent) = 0;
	virtual void logic(float dt) = 0;
};

class EntityManager {
	std::list<Entity*> mNewEntityQueue;
	std::list<Entity*> mEntities;
	
	std::list<System*> mLogicSystems;
	std::list<System*> mConstraintSystems;
	
	void addEntityToSystems(Entity* ent)
	{
		for(auto logSys : mLogicSystems)
			if(logSys->caresAbout(ent))
				logSys->addEntity(ent);
				
		for(auto conSys : mConstraintSystems)
			if(conSys->caresAbout(ent))
				conSys->addEntity(ent);
	}
	
	void removeEntityFromSystems(Entity* ent)
	{
		for(auto conSys : mConstraintSystems)
			if(conSys->caresAbout(ent))
				conSys->removeEntity(ent);
				
		for(auto sys : mSystems)
			if(sys->caresAbout(ent))
				sys->removeEntity(ent);
	}
	
	void addQueuedEntities()
	{
		while(!mNewEntityQueue.empty())
		{
			Entity* ent = mNewEntityQueue.front();
			addEntityToSystems(ent);
			mEntities.push_back(ent);
			mNewEntityQueue.pop_front();
		}
	}
	
public:
	EntityManager()
	{
	}
	
	~EntityManager()
	{
		clearEntities();
		clearSystems();
	}
	
	void clearEntities()
	{
		for(auto ent : mEntities)
			delete ent;
		mEntities.clear();
	}
	
	void clearSystems()
	{
		for(auto sys : mSystems)
			delete sys;
		mSystems.clear();
	}
	
	void clearConstraints()
	{
		for(auto cons : mConstraints)
			delete cons;
		mConstraints.clear();
	}
	
	void addEntity(Entity* ent)
	{
		mNewEntityQueue.push_back(ent);
	}
	
	void logic(float dt)
	{
		addQueuedEntities();
		
		for(auto iter = mEntities.begin(); iter != mEntities.end(); )
		{
			Entity* ent = *iter;
			
			if(ent->getRequestedDeath() && ent->getFramesSinceDeathRequest() > 0)
			{
				removeEntityFromSystems(ent);
				mEntities.erase(iter);
				delete ent;
			}
			
			else
			{
				ent->logic(dt);
				++iter;
			}
		}
	}
};

COMPONENT_START(TestComp)
COMPONENT_END




//void mainLogicLoop()
//{
//	if(object.isDead()) // c1
//		doSomething();
//	
//	object.kill();
//	
//	if(object.isDead())
//		removeAndDelete(object);
//		
//	// Problem:
//	//	c1 will attempt to read a field from a freed object.
//	
//	// Solutions:
//	// 1. delay removal and destruction by one frame. Systems ignore entities if death flag is set.
//	//		Cons: Referring objects have to check if the referred to entity is dead each frame.
//	// 2. proxy with smart ptr. 
//	//		Cons: possibly slow.
//	// 3. shared_ptr
//	//		Cons: no single chunk of code responsible for freeing the object.
//	// 4. no direct reference. Assign each entity unique id. Do lookup/search by name or id
//	//		Cons: possibly slow.
//	// Conclusion: avoid keeping references to other entities, use 2 or 4 when unavoidable!
//}

template <typename T>
struct ChangeTracker {
	T old;
	T* trackPtr;
	
	// Problem: relies on continued existence of tracked field.
	// Solutions:
	// 1. 
	
	ChangeTracker(T* ptr)
	{
		old = *ptr;
		trackPtr = ptr;
	}
	
	bool changed()
	{
		bool res = (old != *trackPtr);
		old = *trackPtr;
		return res;
	}
};

int main()
{
    bool exit = false;
    
    init();
    
    ChangeTracker<volatile int> ct(&mouse_b);
    ChangeTracker<volatile int> ct2(&mouse_x);
    
    printf("%s\n", TestComp::getClassNameStatic());
    printf("%s\n", TestComp().getClassName());
    
    Entity ent;
    ent.addComponent(new TestComp());
    TestComp* tc = ent.getComponent<TestComp>();
    
    printf("%s\n", tc->getClassName());
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     if(ct.changed())
        printf("%d\n", mouse_b);
     if(ct2.changed())
       printf("%d\n", mouse_x);
	 
     clear_to_color(buffer, makecol(64, 64, 64));
    
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
