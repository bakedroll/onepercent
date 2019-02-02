control:on_event(defines.callback.on_initialize, function()

  visuals:bind_branch_value_to_visuals("politics", "propagated", "takeover")

end)

control:on_event(defines.callback.on_country_changed, function(cid)

	log:debug("COUNTRY CHANGED")

end)