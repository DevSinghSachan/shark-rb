module MatrixInspector
	def inspect
		original_inspect = super()
		original_inspect = original_inspect[0..original_inspect.size-2]
		original_inspect + " " + inspect_helper.join(" ") + ">"
	end

protected

	def inspect_helper
		["@data =","[" + self.to_a.map {|i| "["+i.join(", ") + "]"}.join(" ; ")+ "]"]
	end
end

class Optimizer::RealMatrix
	include(MatrixInspector)
end
class Optimizer::RealMatrixReference
	include(MatrixInspector)
end

class Float
	alias_method :old_multiply, :*
	def *(other)
		[
			Optimizer::RealVector, Optimizer::RealVectorReference, Optimizer::RealMatrix, Optimizer::RealMatrixReference, Optimizer::RealMatrixRow, Optimizer::RealMatrixColumn
		].include?(other.class) ? (other * self) : self.old_multiply(other)
	end
end

class Fixnum
	alias_method :old_multiply, :*
	def *(other)
		[
			Optimizer::RealVector, Optimizer::RealVectorReference, Optimizer::RealMatrix, Optimizer::RealMatrixReference, Optimizer::RealMatrixRow, Optimizer::RealMatrixColumn
		].include?(other.class) ? (other * self) : self.old_multiply(other)
	end
end

class Array
	alias_method :old_sum, :+

	def +(other)
		[
			Optimizer::RealVector, Optimizer::RealVectorReference, Optimizer::RealMatrix, Optimizer::RealMatrixReference, Optimizer::RealMatrixRow, Optimizer::RealMatrixColumn
		].include?(other.class) ? (other + self) : self.old_sum(other)
	end
end

module ArrayInspector
	def inspect
		original_inspect = super()
		original_inspect = original_inspect[0..original_inspect.size-2]
		original_inspect + " " + inspect_helper.join(" ") + ">"
	end

protected

	def inspect_helper
		if self.length > 10
			ar = self.to_a
			["@data =","[" + ar[0..4].join(", ") + "..." + ar[-5..-1].join(", ") + "]"]
		else
			["@data =","[" + self.to_a.join(", ") + "]"]
		end
	end
end

class Optimizer::RealVector
	include(ArrayInspector)
end
class Optimizer::RealVectorReference
	include(ArrayInspector)
end
class Optimizer::RealMatrixRow
	include(ArrayInspector)
end
class Optimizer::RealMatrixColumn
	include(ArrayInspector)
end