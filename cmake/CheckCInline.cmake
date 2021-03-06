  # Copyright (c) 2012-2013 United States Government as represented by
  # the U.S. Army Research Laboratory.
  #
  # Redistribution and use in source and binary forms, with or without
  # modification, are permitted provided that the following conditions
  # are met:
  #
  # 1. Redistributions of source code must retain the above copyright
  # notice, this list of conditions and the following disclaimer.
  #
  # 2. Redistributions in binary form must reproduce the above
  # copyright notice, this list of conditions and the following
  # disclaimer in the documentation and/or other materials provided
  # with the distribution.
  #
  # 3. The name of the author may not be used to endorse or promote
  # products derived from this software without specific prior written
  # permission.
  #
  # THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
  # OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  # WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  # ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
  # DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  # DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  # GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  # INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
  # WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  # NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  # SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  #
  ###
  #
  # This file provides a NSR_CHECK_C_INLINE macro that detects what keyword,
  # if any, is used for inlining C functions. This check is similar to
  # autoconf's AC_C_INLINE macro. The macro tests the inline keyword
  # (c99), then __inline__ (c89), and then __inline. When it finds one
  # that works, it will cache it to HAVE_INLINE and return it in the
  # provided RESULT variable. If none work, it will set HAVE_INLINE and
  # RESULT to an empty string. Individual tests are also stored in the
  # cache as HAVE_${INLINE}_KEYWORD variables.
  #
  # Common usage:
  #
  # NSR_CHECK_C_INLINE (C_INLINE)
  # if (NOT C_INLINE STREQUAL "inline")
  #   add_definitions ("-Dinline=${C_INLINE}")
  # endif ()
  #
  # This implementation was developed for BRL-CAD, initially motivated
  # by a snippet from Jack Kelly on the CMake mailing list in Sep 2007,
  # and further inspired by GNU Autoconf's c.m4.
  # Modified by Alexander Saprykin.
  #
  ###
  
include (CheckCSourceCompiles)

macro (NSR_CHECK_C_INLINE RESULT)
	if (NOT DEFINED HAVE_INLINE)
		# Initialize to empty
		set (${RESULT} "")

		# Test candidates to find one that works
		foreach (INLINE "inline" "__inline__" "__inline")
			if (NOT HAVE_INLINE)
				set (PRE_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
				set (CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -Dinline=${INLINE}")

				check_c_source_compiles ("
					typedef int foo_t;
					static inline foo_t static_foo () {
						return 0;
					}
					foo_t foo () {
						return 0;
					}
					int main () {
						return 0;
					}"
					HAVE_INLINE_KEYWORD
				)

				set (CMAKE_REQUIRED_FLAGS "${PRE_CMAKE_REQUIRED_FLAGS}")

				if (HAVE_INLINE_KEYWORD)
					set (HAVE_INLINE "${INLINE}" CACHE INTERNAL "C compiler provides inlining support")
				endif()
			endif()
		endforeach()
	endif()

	if (NOT DEFINED HAVE_INLINE)
		set (HAVE_INLINE "" CACHE INTERNAL "C compiler does not provide inlining support")
	endif()

	# Return the final verdict
	set (${RESULT} ${HAVE_INLINE})
endmacro()
