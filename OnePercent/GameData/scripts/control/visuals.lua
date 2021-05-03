local countries = lua.countries
local visuals   = lua.visuals
local control   = lua.control
local model     = model

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

local color_selected_border = Vec3f(1.0, 0.53, 0.067)

local country_text_node

local function set_country_node_color(cid, color, neighbourcolor)

	if cid == 0 then return end

	local node = countries:get_country_node(cid)

	node:set_uniform_vec4f("overlayColor", color)

	local neighbours = node:get_neighbours()
	for _, neighbour_node in ipairs(neighbours) do
		if neighbour_node then
			neighbour_node:set_uniform_vec4f("overlayColor", neighbourcolor)
		end
	end

end

control:on_event(defines.callback.on_tick, function()

	if country_text_node == nil then return end

	local cid = countries:get_current_country_id()
	if cid == 0 then return end

	local dept_val = model.state[cid].values["ideology"]
	country_text_node:set_text("Ideology: " .. dept_val)

end)

control:on_event(defines.callback.on_initialize, function()

	local presenters = countries:get_country_presenters()
	for _, presenter in pairs(presenters) do
		presenter:make_boundaries(color_selected_border, 0.015)
		presenter:set_boundaries_enabled(false)
	end

end)

control:on_event(defines.callback.on_country_changed, function(old_cid, cid)

	if old_cid > 0 then
		local presenter = countries:get_country_presenter(old_cid)
		presenter:set_boundaries_enabled(false)
		presenter:clear_nodes()

		country_text_node = nil
		-- presenter:remove_node_bin("marker")
	end

	if cid > 0 then
		local presenter = countries:get_country_presenter(cid)

		presenter:set_boundaries_enabled(true)

		-- local node = visuals:get_model_prototype("prototype_sphere")
		-- presenter:scatter_nodes_to_bin(node, "marker", 1.0)

		country_text_node = visuals:create_text_node("")
		presenter:add_node(country_text_node)
	end

	set_country_node_color(old_cid, color_transparent, color_transparent)
	set_country_node_color(cid,	    color_selected,    color_neighbour)


end)

control:on_event(defines.callback.on_overlay_changed, function(old_branch, branch)

	countries:set_uniform_vec4f("takeoverColor",
		(branch == '') and color_transparent or colors_overlay[branch])

	if old_branch ~= '' then
		visuals:unbind_group_value(old_branch, "propagated")
	end

 	if branch ~= '' then
		visuals:bind_group_value_to_visuals(branch, "propagated", "takeover")
	end

	visuals:update_bindings()

end)