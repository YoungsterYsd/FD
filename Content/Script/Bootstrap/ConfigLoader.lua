-- [[ ConfigLoader.lua — JSON 桥接方式，消除 UnLua table→struct 警告 ]]

local ConfigManifest = require("Config.ConfigManifest")

-- 简单 JSON 序列化器（处理平面 table: number 和 string 值）
local function ToJson(tbl)
    local parts = {}
    for k, v in pairs(tbl) do
        local val
        if type(v) == "number" then
            val = tostring(v)
        else
            val = '"' .. tostring(v) .. '"'
        end
        table.insert(parts, '"' .. k .. '": ' .. val)
    end
    return '{' .. table.concat(parts, ', ') .. '}'
end

local M = {}

function M.LoadConfigs(GameInstance)
    local Subsys = UE.UFDConfigSubsystem.Get(GameInstance)
    if not Subsys then
        UE.UKismetSystemLibrary.PrintString(GameInstance,
            "[ConfigLoader] ERROR: Failed to get UFDConfigSubsystem",
            true, true, UE.FLinearColor(1, 0, 0, 1), 10.0, UE.FName())
        return false
    end

    local LoadedCount = 0
    local FailedModules = {}
    local TotalRows = 0

    for _, entry in ipairs(ConfigManifest) do
        local ok, data = pcall(require, entry.Module)
        if not ok then
            table.insert(FailedModules, entry.Module .. " (require failed)")
            goto continue
        end
        if type(data) ~= "table" then
            table.insert(FailedModules, entry.Module .. " (not a table)")
            goto continue
        end

        local RowCount = 0
        for id, row in pairs(data) do
            local numId = tonumber(id)
            if numId then
                local json = ToJson(row)
                local writeOk = pcall(Subsys.WriteConfigRowJson, Subsys, numId, entry.StructPath, json)
                if writeOk then
                    RowCount = RowCount + 1
                end
            end
        end

        if RowCount > 0 then
            LoadedCount = LoadedCount + 1
            TotalRows = TotalRows + RowCount
        else
            table.insert(FailedModules, entry.Module .. " (0 rows)")
        end
        ::continue::
    end

    local TotalCount = #ConfigManifest
    local Msg = string.format("[ConfigLoader] Loaded %d/%d config types, %d total rows",
        LoadedCount, TotalCount, TotalRows)
    local Color
    if #FailedModules > 0 then
        Msg = Msg .. " | FAILED: " .. table.concat(FailedModules, ", ")
        Color = UE.FLinearColor(1, 0.2, 0, 1)
    else
        Color = UE.FLinearColor(0, 1, 0, 1)
    end
    UE.UKismetSystemLibrary.PrintString(GameInstance, Msg, true, false, Color, 5.0, UE.FName())

    return #FailedModules == 0
end

return M
