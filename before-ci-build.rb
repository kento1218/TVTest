require 'httpclient'

def fetch_base_classes
  client = HTTPClient.new
  url = ENV['BASE_CLASSES_URL']
  fn = 'base_classes.zip'
  File.open(fn, 'wb') do |out|
    client.get_content(url, follow_redirect: true) do |chunk|
      out.write chunk
    end
  end

  `7z e -y -osrc/BaseClasses base_classes.zip`
end

def update_revision(rev)
  filename = 'src/TVTest.h'
  lines = File.open(filename, encoding: 'sjis').to_a
  File.open(filename, 'w', encoding: 'sjis') do |f|
    lines.each do |l|
      l.gsub! /VERSION_REVISION\s[0-9]*/, "VERSION_REVISION\t#{rev}"
      f.print l
    end
  end
end

if __FILE__ == $0
  fetch_base_classes
  update_revision(ENV['APPVEYOR_BUILD_NUMBER'])
end
