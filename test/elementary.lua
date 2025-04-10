-- This is a comment
local function main() do
    local x = 10
    if x then
        -- Declarations without `local` are fine, too.
        y = x - 100
        print(138 + (y - 6))
    else
        print(0)
    end

    -- print(y - 1)
end
