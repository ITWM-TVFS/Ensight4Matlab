%% getPartName(object, part)
% Returns a part's name
%
% INPUT
%  object   : |EnsightLib| object
%  part     : (int) 
%
% OUTPUT
%  name     : (string) name
%
% USAGE
%  name = object.getName(part)
%  name = object.getName() (only one part exists)
%

%%
function name = getPartName(this, part)
    assert(this.getNumberOfParts()>0,'EnsightLib::getPartName - No |EnsightPart| has been created so far.');
    if nargin == 1
        assert(this.getNumberOfParts()==1,'EnsightLib::getPartName - There exists more than one |EnsightPart|. A part_id is required. Type `help EnsightLib.getPartName` for further information.');
        name = this.EnsightPartList{1,2};
    else
        pid = this.verifyPartIdentifier(part);
        assert(pid>=0,'EnsightLib::getPartName - Invalid part_id. Type `help EnsightLib.getPartName` for further information.');
        name = this.EnsightPartList{this.getPartIndex(pid),2};
    end
end