SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		Milrow
-- Create date: 
-- Description:	
-- =============================================
CREATE PROCEDURE SP_CharacterList 
	@accountId bigint
AS
BEGIN
	SET NOCOUNT ON;
	SELECT 
	CharacterID, 
	Name, 
	Slot, 
	CreatedAt 
	FROM dbo.[Character] 
	WHERE AccountID = @accountId 
	ORDER BY Slot ASC
END
GO
