%% createEnsightPart(object, name, id)
% Creates new |EnsightPart|.
%
% INPUT
%  object    : |EnsightLib| object
%  name      : part name (string, optional)
%  id        : part id (integer, optional)
%
% OUTPUT
%  part_id   : (integer, optional) id of the new EnsightPart
%  part_name : (string, optional) name of the new EnsightPart
%
% USAGE
%  part = object.createEnsightPart()
%  part = object.createEnsightPart(name)
%  part = object.createEnsightPart(namename, id)
%  

%%
function varargout = createEnsightPart(this, name, id)
    valueArray = [3, 0, 5, 1, 0];
    
      % Generate default values for optional parameters (name, id)
      if nargin < 3
          id = this.getMaxId()+1;
      end
      if nargin < 2
          name = ['Part_',num2str(id)];
      end
      
      % Check input
      assert(isa(this,'EnsightLib'),'EnsightLib::createEnsightPart - Input1(object) is not valid. Type `help EnsighLib.createEnsightPart` for detailed information');
      assert(isa(name,'char'),'EnsightLib::createEnsightPart - Input2(name) is not valid. Type `help EnsighLib.createEnsightPart` for detailed information');
      assert(isnumeric(id),'EnsightLib::createEnsightPart - Input3(id) is not valid. Type `help EnsighLib.createEnsightPart` for detailed information');
      assert(this.inEditMode(),'EnsightLib::createEnsightPart - Not in edit mode');
     
      % Check uniqueness of identifiers
      assert(~any(cell2mat(this.EnsightPartList(:,1))==id),['EnsightLib::createEnsightPart - An EnsightPart with id `',num2str(id),'` already exists.']);
      assert(~any(ismember(this.EnsightPartList(:,2),name)),['EnsightLib::createEnsightPart - An EnsightPart with name `',name,'` already exists.']);
      
      % Position of next element
      n = size(this.EnsightPartList,1)+1;
      
      % Invoke Cpp-Constructor
      EnsightLib_interface('obj', 'createPart', valueArray, this.getObjectHandle(), name, id);
      
      % Set Attributes
      this.EnsightPartList{n,1} = id;
      this.EnsightPartList{n,2} = name;
      
      
      if nargout == 2
          varargout{1} = id;
          varargout{2} = name;
      elseif nargout == 1
          varargout{1} = id;
      end
     
end