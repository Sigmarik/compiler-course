-- This is a comment
local function main() do
    local x = 10
    local condition = x == 0
    if condition then
        print(0)
    else
        -- Declarations without `local` are fine, too.
        y = x - 100
        print(138 + (y - 6))
    end

    -- print(y - 1)
end
