# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/code_and_study/DUT/PBL2_Clion/cmake-build-debug/_deps/ftxui-src"
  "D:/code_and_study/DUT/PBL2_Clion/cmake-build-debug/_deps/ftxui-build"
  "D:/code_and_study/DUT/PBL2_Clion/cmake-build-debug/_deps/ftxui-subbuild/ftxui-populate-prefix"
  "D:/code_and_study/DUT/PBL2_Clion/cmake-build-debug/_deps/ftxui-subbuild/ftxui-populate-prefix/tmp"
  "D:/code_and_study/DUT/PBL2_Clion/cmake-build-debug/_deps/ftxui-subbuild/ftxui-populate-prefix/src/ftxui-populate-stamp"
  "D:/code_and_study/DUT/PBL2_Clion/cmake-build-debug/_deps/ftxui-subbuild/ftxui-populate-prefix/src"
  "D:/code_and_study/DUT/PBL2_Clion/cmake-build-debug/_deps/ftxui-subbuild/ftxui-populate-prefix/src/ftxui-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/code_and_study/DUT/PBL2_Clion/cmake-build-debug/_deps/ftxui-subbuild/ftxui-populate-prefix/src/ftxui-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/code_and_study/DUT/PBL2_Clion/cmake-build-debug/_deps/ftxui-subbuild/ftxui-populate-prefix/src/ftxui-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
