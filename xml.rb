require 'rexml/document'

#branches = ARGV
branches = [
	'http://svn.apache.org/repos/asf/subversion/branches/1.8.x',
	'http://svn.apache.org/repos/asf/subversion/branches/1.7.x',
]

master_doc = REXML::Document.new(<<EOS)
<?xml version="1.0?>
<log>
</log>
EOS


branches.each do |branch|
	IO.popen("svn log -l 3 --xml #{branch}", "r+") do |io|
		#puts io.gets until io.eof?

		doc = REXML::Document.new(io)
		doc.elements.each('/log/logentry') do |element|
			branch_elem = REXML::Element.new('branch')
			branch_elem.add_attribute('name', branch[/branches\/.*/])
			branch_elem.text = "Commit"
			element.add_element(branch_elem)

			master_doc.root.add_element(element)
		end
	end
end

master_doc.root.get_elements('/log/logentry').sort! do |element1, element2|
	element1.attribute("revision").value <=> element2.attribute("revision").value
end

puts master_doc

branches.each do |src|
	branches.each do |target|
		if src != target then
			puts src
			puts target

			puts "merged"
			IO.popen("svn mergeinfo --show-revs merged #{src} #{target}", "r+") do |io|
			end
			puts "eligible"
			IO.popen("svn mergeinfo --show-revs eligible #{src} #{target}", "r+") do |io|
			end
		end

	end
end

exit
