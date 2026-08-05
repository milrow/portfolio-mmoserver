SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		Milrow
-- Create date: 
-- Description:	
-- =============================================
CREATE PROCEDURE SP_UpdateCharacterPosition 
	-- Add the parameters for the stored procedure here
	@characterId bigint = 0, 
	@mapId int = 0,
	@posX float = 0,
	@posY float = 0,
	@posZ float = 0,
	@yaw float = 0
AS
BEGIN
	SET NOCOUNT ON;
	UPDATE dbo.[CharacterPosition] 
	SET PosX = @posX, PosY = @posY, PosZ = @posZ, Yaw = @yaw, MapID = @mapId, UpdatedAt = GETDATE() 
	WHERE CharacterID = @characterId;

	IF @@ROWCOUNT = 0
		BEGIN
			SELECT 1 AS ResultCode; 
			RETURN;
		END

	SELECT 0 AS ResultCode;
END
GO
