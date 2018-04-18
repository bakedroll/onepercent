-- contains functions for debug purposes e.g. printing values

core.helper = {}

-- dumping table to a string
function core.helper.dump_object(o, d)
  if d == nil then
    d = 0
    core.helper.visited_objects = {}
  end

  local s = ''
  if type(o) == 'table' then
    if core.helper.visited_objects[o] then
      s = s .. '#REF#'
    else
      core.helper.visited_objects[o] = true

      s = s .. '{ '
      for k,v in pairs(o) do
        if type(k) ~= 'number' then k = '"'..k..'"' end
        s = s .. '\n'
        for i=0,d do s = s .. '  ' end
        s = s .. '['..k..'] = ' .. core.helper.dump_object(v, d + 1) .. ','
      end
      s = s .. ' }'
    end
  else
    s = s .. tostring(o)
  end

  if d == 0 then core.helper.visited_objects = nil end
  return s

end

-- copies a table value by value
function core.helper.deepcopy(orig)

  local orig_type = type(orig)
  local copy
  if orig_type == 'table' then
      copy = {}
      for orig_key, orig_value in next, orig, nil do
          copy[core.helper.deepcopy(orig_key)] = core.helper.deepcopy(orig_value)
      end
      setmetatable(copy, core.helper.deepcopy(getmetatable(orig)))
  else -- number, string, boolean, etc
      copy = orig
  end
  return copy

end