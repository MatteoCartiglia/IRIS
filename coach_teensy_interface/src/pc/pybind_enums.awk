# Invoke with 'awk -v enum=EnumClassName' to extract to stdout a pybind11
# py::enum_ definition of enum class EnumClassName from the C++ file on stdin.
# Use -v scope=AnotherClass on the command line if the enum is enclosed in
# AnotherClass.

BEGIN {
enum_start = "^enum class " enum " {"
enum_stop = "};"
body = 0
if (scope == "") {print "py::enum_<" enum ">(m, \"" enum "\")"}
else {print "py::enum_<" scope "::" enum ">(" tolower(scope) ", \"" enum "\")"}
}

$0 ~ enum_start {body = 1; next}
$0 ~ enum_stop {body = 0; next}
body == 1 && scope == "" {print ".value(\"" $1 "\", " enum "::" $1 ")"}
body == 1 && scope != "" {print ".value(\"" $1 "\", " scope "::" enum "::" $1 ")"}

END {
print ";"
}
