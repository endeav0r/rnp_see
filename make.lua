#!/usr/bin/lua

--require('lfs')

CPP   = 'g++'
FLAGS = '-Wall -Wno-switch -g --std=c++0x'
COMPILE = CPP .. ' ' .. FLAGS .. ' -c '

function execute_try (command)
    print('trying: ' .. command)
    return os.execute(command)
end

function execute (command)
    print(command)
    local status = os.execute(command)
    if status ~= 0 then 
        os.exit(status)
    end
end

function check_src_times (dep)
    local err, o_time = pcall(function () return lfs.attributes('src/' .. dep .. '.o').modification end)
    if not err then return true end
    
    local cc_time = lfs.attributes('src/' .. dep .. '.cc').modification
    local h_time  = lfs.attributes('src/' .. dep .. '.h').modification
    if cc_time > o_time or h_time > o_time then
        return true
    else
        return false
    end
end

function do_config ()
    HEADERS = {}
    HEADERS['lua.h'] = {'', 'lua51/', 'lua50/'}

    HEADER_PATHS = {}

    for filename, paths in pairs(HEADERS) do
        header_found = false
        for i,path in pairs(paths) do
            local command = 'echo "#include <' .. path .. filename .. '>" | '
            command = command .. 'gcc -x c -c - -o /dev/null'
            if execute_try(command) == 0 then
                print('found ' .. path .. filename)
                table.insert(HEADER_PATHS, '#include <' .. path .. filename .. '>\n')
                header_found = true
                break
            end
        end
        if not header_found then
            print('could not find header file: ' .. filename)
            os.exit(1)
        end
    end

    fh = io.open('src/config.h', 'w')
    fh:write(table.concat(HEADER_PATHS, '\n'))
    fh:close()
end

deps = {'vm', 'symbolicvalue', 'translator', 'block', 'debug', 
        'elf', 'instruction', 'memory', 'page'}
dep_objs = {}

for di, dep in pairs(deps) do
    if check_src_times(dep) then
        execute(COMPILE .. " -o src/" .. dep .. ".o src/" .. dep .. ".cc")
    end
    table.insert(dep_objs, 'src/' .. dep .. '.o')
end

execute(CPP .. " " .. FLAGS .. " -o see -L/usr/local/lib src/see.cc " .. table.concat(dep_objs, " ") .. " -ludis86")
