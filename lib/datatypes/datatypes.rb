class Optimizer::RealMatrix
	def inspect
		original_inspect = super()
		original_inspect = inspected[0..original_inspect.size-1]
		original_inspect + " " + inspect_helper.join(" ") + ">"
	end

protected

	def inspect_helper
		["@data =","[ " + self.to_a.map {|i| "[ "+i.join(", ") + " ]"}.join("; ") + "..." + ar[-6..-1].join("; ") + " ]"]
	end

end
class Optimizer::RealVector
	def inspect
		original_inspect = super()
		original_inspect = inspected[0..original_inspect.size-1]
		original_inspect + " " + inspect_helper.join(" ") + ">"
	end

protected

	def inspect_helper
		if self.length > 10
			ar = self.to_a
			["@data =","[ " + ar[0..4].join(", ") + "..." + ar[-5..-1].join(", ") + " ]"]
		else
			["@data =","[ " + self.to_a.join(", ") + " ]"]
		end
	end
end