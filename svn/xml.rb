require 'rexml/document'

#branches = ARGV
branches = [
	'file:///home/yasukei/svn/svn_server_sample/trunk',
	'file:///home/yasukei/svn/svn_server_sample/branches/branch1',
]

master_doc = REXML::Document.new(<<EOS)
<?xml version="1.0?>
<log>
</log>
EOS


branches.each do |branch|
	IO.popen("svn log -l 50 --xml --stop-on-copy #{branch}", "r+") do |io|
		#puts io.gets until io.eof?

		doc = REXML::Document.new(io)
		doc.elements.each('/log/logentry') do |element|
			#branch_elem = REXML::Element.new('branch')
			branch_elem = REXML::Element.new(branch[/(trunk|branches\/.*)/].gsub(/branches\//, ""))
			branch_elem.text = "Commit"
			element.add_element(branch_elem)

			master_doc.root.add_element(element)
		end
	end
end

#master_doc.root.get_elements('/log/logentry').sort! do |element1, element2|
#	element1.attribute("revision").value.to_i <=>  element2.attribute("revision").value.to_i
#end

#puts master_doc

branches.each do |src|
	branches.each do |target|
		if src != target then
			#puts src
			#puts target

			option = "merged"
			text = "Merged"

			IO.popen("svn mergeinfo --show-revs #{option} #{src} #{target}", "r+") do |io|
				io.each_line do |line|
					specific_revision = line.gsub(/^r/, "").chomp

					master_doc.root.each_element('/log/logentry') do |element|
						if element.attribute("revision").value == specific_revision then
							branch_elem = REXML::Element.new(target[/(trunk|branches\/.*)/].gsub(/branches\//, ""))
							#branch_elem.add_attribute('name', target[/(trunk|branches\/.*)/].gsub(/branches\//, ""))
							branch_elem.text = text
							element.add_element(branch_elem)
						end

					end
				end
			end

			option = "eligible"
			text = "Not Merged"

			IO.popen("svn mergeinfo --show-revs #{option} #{src} #{target}", "r+") do |io|
				io.each_line do |line|
					specific_revision = line.gsub(/^r/, "").chomp

					master_doc.root.each_element('/log/logentry') do |element|
						if element.attribute("revision").value == specific_revision then
							branch_elem = REXML::Element.new(target[/(trunk|branches\/.*)/].gsub(/branches\//, ""))
							#branch_elem.add_attribute('name', target[/(trunk|branches\/.*)/].gsub(/branches\//, ""))
							branch_elem.text = text
							element.add_element(branch_elem)
						end

					end
				end
			end

		end

	end
end

puts master_doc

