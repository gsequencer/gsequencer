#!/usr/bin/ruby

require 'optparse'
require 'optparse/time'
require 'ostruct'
require 'pp'

class LibagsTool

  Version = '0.0.1'

  attr_accessor :output_filename, :input_filename,
                :output, :input,
                :destination, :source,
                :options
  
  # default constructor of libags-tool
  def initialize(output_filename, input_filename)

    @output_filename = output_filename
    @input_filename = input_filename

  end
  
  # strips uuid of buffer
  def strip_uuid()

    # strip by regular expression
    @destination = @destination.gsub(/([0-9a-fA-F]+)-([0-9a-fA-F]+)-([0-9a-fA-F]+)-([0-9a-fA-F]+)-([0-9a-fA-F]+)/, '')

    # return the new destination
    @destination
    
  end

  def open(filename, is_output)

    # check if output
    if(is_output)
      # open output
      @output_filename = filename
      @output = File.new(filename, "w")
    else
      # open input
      @input_filename = filename
      @input = File.new(filename, "r")
    end

  end

  # read input
  def read_buffer()
    # read input to source as string
    @source = IO.read(@input_filename)

    # duplicate the string and use it as destination string
    @destination = String.new(@source)

    # return source string
    @source
  end

  # write output
  def write_buffer()

    # write destination string to output
    @output << @destination

    # return destination string
    @destination
  end

  # close all file descriptors
  def close_all()
    @output.close()
    @input.close()
  end

  class LibagsOptions

    attr_accessor :output_filename, :input_filename
    
    # default constructor of LibagsOptions
    def initialize()
      self.output_filename = "/dev/stdout"
      self.input_filename = nil
    end

  end

  # parse options
  def self.parse(args)

    tmp = nil
    
    has_output = false
    has_input = false

    # check arguments not empty
    if(args.length == 0)
      print_usage()      
      exit -1
    end

    # instantiate options
    options = LibagsOptions.new()

    # iterate options
    for tmp in args

      specifier = nil
      
      option_index = -1
      
      success = false

      # check option
      case tmp
      when "-v"

        # print version
        print_version()
        exit 0
        
      when "-o"

        # output filename
        option_index = args.index("-o")
          
        # check if one more argument is provided
        if(!has_output &&
           option_index + 1 < args.length)
          
          # output file
          specifier = args[option_index + 1]

          # check if file doesn't exist
          if(!FileTest.exists?(specifier))
            options.output_filename = specifier

            # success
            has_output = true
            success = true
          else
            success = false
          end

        else
          success = false
        end

      when "-i"

        # input filename
        option_index = args.index("-i")

        # check if one more argument is provided
        if(!has_input &&
           option_index + 1 < args.length)
          
          # input file
          specifier = args[option_index + 1]

          # check if file does exist
          if(FileTest.exists?(specifier))
            options.input_filename = specifier

            # success
            has_input = true
            success = true
          else
            success = false
          end
          
        else
          success = false
        end
      else
        success = true
      end

      # print usage and exit
      if(!success)
        print_usage()
        exit -1
      end

    end

    # return options
    options
  end
  
  def self.print_usage()

    printf("libags-tool v#{Version}\n")
    printf("libags-tool -i source -o </dev/stdout|destination>\n")
    printf("Options:\n")
    version_option()
    input_option()
    output_option()

  end

  def self.print_version()

    printf("#{Version}\n")      

  end

  def self.version_option()

    printf("-v\t\t\tShow version information and exit\n")

  end

  def self.input_option()

    printf("-i\t\t\tRequired input filename to read from.\n")

  end

  def self.output_option()

    printf("-o\t\t\tOutput filename to safe your output. Optional.\n")

  end
  
  public :strip_uuid, :open, :read_buffer, :write_buffer, :close_all

end

# instantiate libags_tool and open files
libags_tool = LibagsTool.new(nil, nil)
libags_tool.options = LibagsTool.parse(ARGV)
libags_tool.open(libags_tool.options.output_filename, true)
libags_tool.open(libags_tool.options.input_filename, false)

# read
libags_tool.read_buffer()

# strip and write
libags_tool.strip_uuid()
libags_tool.write_buffer()

# close all
libags_tool.close_all()

0
