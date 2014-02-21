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

class Array
	alias_method :old_sum, :+

	def +(other)
		other.class == Optimizer::RealVector ? (other + self) : self.old_sum(other)
	end

	alias_method :old_product, :+ # could do the same for scalars.
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