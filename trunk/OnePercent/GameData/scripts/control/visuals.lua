control:on_event(defines.callback.on_initialize, function()

  -- visuals:bind_branch_value_to_visuals("politics", "propagated", "takeover")

end)

control:on_event(defines.callback.on_country_changed, function(cid)

	local node = countries:get_country_node(cid)
	if node == nil then return end

	local neighbours = node:get_neighbours()

	log:debug("COUNTRY CHANGED: " .. node:get_name())
	log:debug(helper.dump_object(neighbours))

	node:set_uniform_float("takeover", 0.5)
	node:set_uniform_vec3f("overlayColor", Vec3f(1.0, 0.0, 1.0))

end)

control:on_event(defines.callback.on_overlay_changed, function(branchId)

	log:debug("OVERLAY CHANGED: " .. branchId)

end)