-- This is a comment
local function main() do
    local x = 10
    if x == 0 then
        print(0)
    else
        -- Declarations without `local` are fine, too.
        y = x - 100
        print(138 + (y - 6))
    end

    -- print(y - 1)
end
