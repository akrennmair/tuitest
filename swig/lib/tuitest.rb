require 'tuitest.so'

module Tuitest

	def Tuitest.gettext(row, col, len)
		getrow(row)[col..col+len-1]
	end

	class Verifier

		def initialize(logfile = nil)
			@verifications = [ ]
			@logfile = logfile
			@errcount = 0
			@warncount = 0
		end

		def expect(row, col, expected_text, failmethod = :hard)
			found_text = Tuitest.gettext(row, col, expected_text.length)
			if found_text != expected_text then
				case failmethod
				when :hard
					@verifications << [ :error, "On (#{row},#{col}), expected `#{expected_text}', but found `#{found_text}'" ]
					@errcount += 1
					Tuitest.close
					finish
					Kernel.exit(1)
				when :soft
					@verifications << [ :warn, "On (#{row},#{col}), expected `#{expected_text}', but found `#{found_text}'" ]
					@warncount += 1
				end
			else
				@verifications << [ :ok, "On (#{row},#{col}), found `#{found_text}' as expected." ]
			end
		end

		def finish
			write_log_to_file(@logfile)
			if @errcount > 0 or @warncount > 0 then
				write_log_to_stdout
			end
			puts "Test run finished (#{@errcount} errors, #{@warncount} warnings)."
		end

		private

		def write_log_to_stdout
			write_log_to_stream($stdout)
		end

		def write_log_to_file(file)
			File.open(file,"w+") { |f| write_log_to_stream(f) }
		end

		def write_log_to_stream(stream)
			@verifications.each do |v|
				level = v[0]
				text = v[1]
				case level
				when :ok
					stream.puts("I: #{text}")
				when :warn
					stream.puts("W: #{text}")
				when :error
					stream.puts("E: #{text}")
				end
			end
		end

	end

end
