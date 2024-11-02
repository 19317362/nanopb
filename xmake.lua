set_project("nanopb")
set_version("1.0.0")

add_rules("mode.debug", "mode.release")
-- 如果不是 arm arm64 target 则默认为 debug 模式
if not is_plat("arm", "arm64") then
    set_defaultmode("debug")
end

-- 设置编译选项
set_languages("c99")

-- 设置编译模式
if is_mode("debug") then
    set_symbols("debug")
    add_defines("DEBUG")
else
    set_optimize("fastest")
end

-- 设置编译器选项
if is_plat("windows") and is_kind("static") then
    add_cxflags("/MT")
end

-- 添加目标
target("protobuf-nanopb")
    set_kind("$(kind)")
    add_files("pb_common.c", "pb_encode.c", "pb_decode.c")
    add_headerfiles("pb.h", "pb_common.h", "pb_encode.h", "pb_decode.h",  {prefixdir = "yw_nanopb", public = true})
    -- 如果是 windows + shared 导出所有的符号
    if is_plat("windows") and is_kind("shared") then
        add_rules("utils.symbols.export_all", {export_classes = true})
    end
    -- 如果是 linux + shared 配置rpath
    if is_kind("shared") and is_plat("linux") then
        on_load(function (target)
            for _, pkg in ipairs(target:orderpkgs()) do
                for _, linkdir in ipairs(pkg:get("linkdirs")) do
                    target:add("ldflags", "-Wl,-rpath-link=" .. linkdir, { public = true, force = true })
                end
            end            
        end)     
    end
-- wps 目录中的文件是一个 binary 测例
target("wps_pb")
    set_kind("binary")
    add_files("wps/*.c")
    add_includedirs(".")
    add_deps("protobuf-nanopb")

