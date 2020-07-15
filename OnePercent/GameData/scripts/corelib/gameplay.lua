gameplay = {}

function gameplay.raise_value(value, max, inc_begin, inc_end, decay)

  if value < max then
    local raise = inc_end + ((inc_begin - inc_end) * (1.0 - (value / max)) ^ decay)
    value = value + raise
  else
    value = value + inc_end
  end

  return value

end
