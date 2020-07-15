-- contains functions for debug purposes e.g. printing values

helper = {}

function helper.merge(t1, t2)

  for k, v in pairs(t2) do
    if (type(v) == "table") and (type(t1[k] or false) == "table") then
      helper.merge(t1[k], t2[k])
    else
      t1[k] = v
    end
  end

end

-- dumping table to a string
function helper.dump_object(o, d)
  if d == nil then
    d = 0
    helper.visited_objects = {}
  end

  local s = ''
  if type(o) == 'table' then
    if helper.visited_objects[o] then
      s = s .. '#REF#'
    else
      helper.visited_objects[o] = true

      s = s .. '{ '
      for k,v in pairs(o) do
        if type(k) ~= 'number' then k = '"'..k..'"' end
        s = s .. '\n'
        for i=0,d do s = s .. '  ' end
        s = s .. '['..k..'] = ' .. helper.dump_object(v, d + 1) .. ','
      end
      s = s .. ' }'
    end
  else
    s = s .. tostring(o)
  end

  if d == 0 then helper.visited_objects = nil end
  return s

end

-- copies a table value by value
function helper.deepcopy(orig)

  local orig_type = type(orig)
  local copy
  if orig_type == 'table' then
      copy = {}
      for orig_key, orig_value in next, orig, nil do
          copy[helper.deepcopy(orig_key)] = helper.deepcopy(orig_value)
      end
      setmetatable(copy, helper.deepcopy(getmetatable(orig)))
  else -- number, string, boolean, etc
      copy = orig
  end
  return copy

end
