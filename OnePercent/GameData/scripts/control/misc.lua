control:on_event(defines.callback.on_initialize, function()

  log:debug("on_initialize_action_new")

end)

control:on_event(defines.callback.on_tick, function()

  log:debug("on_tick_action")

end)
