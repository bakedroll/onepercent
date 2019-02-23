local countries = countries

local color_transparent = Vec4f(0.0, 0.0, 0.0, 0.0)
local color_selected    = Vec4f(0.5, 0.69, 1.0, 0.5)
local color_neighbour   = Vec4f(0.5, 0.5, 0.5, 0.4)

local colors_overlay = {
	["banks"]    = Vec4f(0.0,   0.0,   0.8,   0.3),
	["concerns"] = Vec4f(0.118, 0.753, 0.208, 0.3),
	["control"]  = Vec4f(0.635, 0.439, 0.031, 0.3),
	["media"]    = Vec4f(0.902, 1.0,   0.357, 0.3),
	["politics"] = Vec4f(0.69,  0.247, 0.624, 0.3)
}

local function set_country_node_color(cid, color, neighbourcolor)

	if cid == 0 then return end

	local node = countries:get_country_node(cid)

	node:set_uniform_vec4f("overlayColor", color)

	local neighbours = node:get_neighbours()
	for _, neighbour_node in ipairs(neighbours) do
		neighbour_node:set_uniform_vec4f("overlayColor", neighbourcolor)
	end

end

control:on_event(defines.callback.on_initialize, function()

  visuals:bind_branch_value_to_visuals("politics", "propagated", "takeover")

end)

control:on_event(defines.callback.on_country_changed, function(old_cid, cid)

	set_country_node_color(old_cid, color_transparent, color_transparent)
	set_country_node_color(cid,	    color_selected,    color_neighbour)

end)

control:on_event(defines.callback.on_overlay_changed, function(old_branch, branch)

	log:debug("OVERLAY CHANGED: " .. branch)

end)