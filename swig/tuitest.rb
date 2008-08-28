require 'tuitest.so'

module Tuitest

	def Tuitest.gettext(row, col, len)
		getrow(row)[col..col+len-1]
	end

end
