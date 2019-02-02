control:on_event(defines.callback.on_initialize, function()

  visuals:bind_branch_value_to_visuals("politics", "propagated", "takeover")

end)

control:on_event(defines.callback.on_country_changed, function(cid)

	local node = countries:get_country_node(cid)
	if node == nil then return end

	log:debug("COUNTRY CHANGED: " .. node:get_name())

end)