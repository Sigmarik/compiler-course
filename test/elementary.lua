local function add1(x) do
    return x + 1
end

-- This is a comment
function main() do
    local x = 10
    if x then
        -- Declarations without `local` are fine, too.
        y = x - add1(99)
        print(138 + (y - 6))
    else
        print(0)
    end

    -- print(y - 1)
end
