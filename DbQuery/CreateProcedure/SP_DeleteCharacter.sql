-- ================================================
-- Template generated from Template Explorer using:
-- Create Procedure (New Menu).SQL
--
-- Use the Specify Values for Template Parameters 
-- command (Ctrl-Shift-M) to fill in the parameter 
-- values below.
--
-- This block of comments will not be included in
-- the definition of the procedure.
-- ================================================
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		MIlrow
-- Create date: 
-- Description:	
-- =============================================
CREATE PROCEDURE SP_DeleteCharacter 
	-- Add the parameters for the stored procedure here
	@accountId bigint = 0, 
	@characterId bigint = 0
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

    DELETE FROM dbo.[Character] WHERE AccountID = @accountId AND CharacterID = @characterId;	
	DELETE FROM dbo.[CharacterInventory] WHERE CharacterID = @characterId;
	DELETE FROM dbo.[CharacterPosition] WHERE CharacterID = @characterId;
	DELETE FROM dbo.[CharacterSkills] WHERE CharacterID = @characterId;
	DELETE FROM dbo.[CharacterStats] WHERE CharacterID = @characterId;
END
GO
