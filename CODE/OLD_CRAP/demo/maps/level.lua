function Item(names, descs, pos, soundResMapPath)

	local itemProxy = Entity.createProxy()
	local item = itemProxy:getObject()
	local sound = SoundComponent.create()
	sound:loadSoundResMap(soundResMapPath)

	item.pos = pos
	item:addComponent(Identity.create(names, descs))
	item:addComponent(sound)
	item:addComponent(Takable.create())
    
	return item
end

function Prop(names, descs, pos)

	local propProxy = Entity.createProxy()
	local prop = propProxy:getObject()

	prop.pos = pos
	prop:addComponent(Identity.create(names, descs))
    
	return prop

end

worldContainer = getWorldContainer()

worldContainer:addChild(Item("key", "", Vec3f(5, 0, 17), "resmaps/key.txt"))
worldContainer:addChild(Prop("chair,stool", "big,wooden", Vec3f(10, 0, 12)))
worldContainer:addChild(Prop("chair,stool", "small,plastic", Vec3f(10, 0, 15)))