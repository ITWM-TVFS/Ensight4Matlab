%% addConstant(object, name, value)
% Creates a new constant 'name' 
%
% INPUT
%  object : |EnsightLib| object
%  name   : constant name (string)
%  value  : constant value (double, vector or matrix)
%
% OUTPUT
%  none
%
% USAGE
%  object.addConstant(name, value)
%

%%
function addConstant(this, name, value)

    assert(nargin == 3, 'EnsightLib::addConstant - Invalid number of input arguments. Type `help EnsightLib.addConstant` for detailed information');
    
    % Check input and edit mode
    assert(isa(this,'EnsightLib'),'EnsightLib::addConstant - Input1(object) is not valid. Type `help EnsighLib.addConstant` for detailed information');
    assert(isa(name,'char'),'EnsightLib::addConstant - Input2(name) is not valid. Type `help EnsighLib.addConstant` for detailed information');
    assert(isnumeric(value),'EnsightLib::addConstant - Input3(value) is not valid. Type `help EnsighLib.addConstant` for detailed information');
    assert(this.inEditMode(),'EnsightLib::addConstant - Not in edit mode');
    assert(~this.hasConstant(name),['EnsightLib::addConstant - A constant named `',name,'` already exists.']);
    
    valueArray = [3, 0, 5, 1, 0];
    EnsightLib_interface('obj', 'addConstant', valueArray, this.getObjectHandle(), name, value);
    n = size(this.Constants,1);
    this.Constants{n+1,1} = name;
    this.Constants{n+1,2} = value;
end