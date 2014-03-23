module MatrixInspector
	def inspect
		original_inspect = super()
		original_inspect = original_inspect[0..original_inspect.size-2]
		original_inspect + " " + inspect_helper.join(" ") + ">"
	end

	def to_s
		"["+self.each_row.map {|i| i.to_s}.join("\n ")+"]"
	end

protected

	def inspect_helper
		["@data =","[" + self.to_a.map {|i| "["+i.join(", ") + "]"}.join(" ; ")+ "]"]
	end
end

class Optimizer::RealMatrix
	include(MatrixInspector)
	include Enumerable
end
class Optimizer::RealMatrixReference
	include(MatrixInspector)
	include Enumerable
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
	alias_method :old_product, :*

	def +(other)
		[
			Optimizer::RealVector, Optimizer::RealVectorReference, Optimizer::RealMatrix, Optimizer::RealMatrixReference, Optimizer::RealMatrixRow, Optimizer::RealMatrixColumn
		].include?(other.class) ? (other + self) : self.old_sum(other)
	end

	def to_blas_type
		self[0].is_a?(Array) ? self.to_matrix : self.to_realvector
	end

	def *(other)
		[
			Optimizer::RealVector, Optimizer::RealVectorReference, Optimizer::RealMatrix, Optimizer::RealMatrixReference, Optimizer::RealMatrixRow, Optimizer::RealMatrixColumn
		].include?(other.class) ? (self.to_blas_type * other) : self.old_product(other)
	end

	# Hadamard product.
	def o(other)
		if [
			Optimizer::RealMatrix, Optimizer::RealMatrixReference
		].include?(other.class) then (self.to_matrix.o(other))
		else raise NoMethodError.new "undefined method `o' for #{self}" end
	end

	# Hadamard product equals.
	def o=(other)
		if [
			Optimizer::RealMatrix, Optimizer::RealMatrixReference
		].include?(other.class) then (self.to_matrix.o=(other))
		else raise NoMethodError.new "undefined method `o=' for #{self}" end
	end
end

module ArrayInspector
	def inspect
		original_inspect = super()
		original_inspect = original_inspect[0..original_inspect.size-2]
		original_inspect + " " + inspect_helper.join(" ") + ">"
	end

	def to_s
		"["+self.each.map {|i| sprintf("%.2f", i)}.join(" ")+"]"
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
	include Enumerable
end
class Optimizer::RealVectorReference
	include(ArrayInspector)
	include Enumerable
end
class Optimizer::RealMatrixRow
	include(ArrayInspector)
	include Enumerable
end
class Optimizer::RealMatrixColumn
	include(ArrayInspector)
	include Enumerable
end