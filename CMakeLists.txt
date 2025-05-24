# CMakeList.txt: проект CMake для Chepgram; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})

set(POSTGRESQL_INCLUDE_DIR "D:/17/include")
set(POSTGRESQL_LIBRARY_DIR "D:/17/lib")
include_directories(
	"E:/boost_1_88_0",
	${POSTGRESQL_INCLUDE_DIR}
)

link_directories(
	${POSTGRESQL_LIBRARY_DIR}
)

project ("Chepgram")

# Добавьте источник в исполняемый файл этого проекта.
add_executable (Chepgram "Chepgram.cpp" "Chepgram.h" "src/db/Database.h" "src/db/Database.cpp")
add_executable (Chepgram_Client "src/client/client.cpp"  "src/client/client.h")
add_executable (Chepgram_NewClient "src/client/NewClient.cpp" "src/client/MainFrame.cpp")

target_link_libraries(Chepgram
	libpq
	ws2_32
	${wxWidgets_LIBRARIES}
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Chepgram PROPERTY CXX_STANDARD 20)
  set_property(TARGET Chepgram_Client PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
